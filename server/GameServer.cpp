#include "GameServer.hpp"

#include <QDebug>
#include <QThread>

#include "Game.hpp"
#include "GameMode.hpp"
#include "common/MsgpackUtils.hpp"
#include "common/VCS.hpp"

GameHandler::GameHandler(GameServer* gs,
                         const std::map<int, ObjectTypeData>& objectTypeData, const std::map<int, TankModuleData>& tankModuleData,
                         std::vector<bt_messages::ToServerMessage_CreateGame_StartingObject> startingObjects,
                         int id, std::string title)
    : QObject(gs), _title(title) {
    auto thread = new QThread();
    connect(thread, &QThread::started, [=]() {
        auto game = new Game(objectTypeData, tankModuleData);
        qInfo() << "Loading objects" << startingObjects.size();

        for (auto& obj : startingObjects) {
            auto go = game->addObject(static_cast<constants::ObjectType>(obj.type()),
                                      {obj.x(), obj.y()}, obj.rotation(), {0, 0});
            if (go) {
                go->second->setSide(obj.side());
            }
        }
        connect(game, &Game::sendMessage, gs, &GameServer::handleGameMessage);
        connect(this, &GameHandler::_addConnection, game, &Game::addConnection);
        connect(this, &GameHandler::_removeConnection, game, &Game::removeConnection);
        connect(this, &GameHandler::_sendMessage, game, &Game::recieveMessage, Qt::QueuedConnection);

        auto gameMode = new IndividualDeathMatch();
        gameMode->setGame(game);
        connect(game, &Game::destroyed, gameMode, &GameMode::deleteLater);
        connect(gameMode, &GameMode::sendMessage, gs, &GameServer::handleGameMessage);
        connect(gameMode, &GameMode::gameOver, game, &Game::end);

        try {
            game->mainloop();
        }
        catch (...) {
            qFatal("Uncaught exception in game mainloop");
        }
        game->deleteLater();
        emit gameOver(id);
        thread->quit();
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void GameHandler::addConnection(int id, std::shared_ptr<bt_messages::ToServerMessage> msg) { emit _addConnection(id, msg); }

void GameHandler::removeConnection(int id) { emit _removeConnection(id); }

void GameHandler::sendMessage(int id, std::shared_ptr<bt_messages::ToServerMessage> msg) { emit _sendMessage(id, msg); }

GameServer::GameServer(const QHostAddress& address, quint16 port) : _objectTypeData(loadObjectTypeData(":/data/objects.json")), _tankModuleData(loadTankModuleData(":/data/tank_modules.json")) {
    ObjectStateRegister::dumpRegistry();
    qDebug() << "Loaded" << _objectTypeData.size() << "object types and" << _tankModuleData.size() << "tank module types";

    _server = new QTcpServer(this);
    if (!_server->listen(address, port)) {
        qWarning() << "Failed to bind to server port";
        return;
    }
    qInfo() << "Server started, listening at" << _server->serverAddress() << ":" << _server->serverPort();

    connect(_server, &QTcpServer::newConnection, this, &GameServer::handleConnection);
}

int GameServer::addGame(std::vector<bt_messages::ToServerMessage_CreateGame_StartingObject> startingObjects, std::string title) {
    auto id = _nextGameId++;
    auto handler = new GameHandler(this, _objectTypeData, _tankModuleData, startingObjects, id, title);
    connect(handler, &GameHandler::gameOver, this, &GameServer::removeGame);
    _games[id] = handler;
    return id;
}

void GameServer::removeGame(int gameId) {
    qInfo() << "Removing game" << gameId;
    _games[gameId]->deleteLater();
    _games.erase(gameId);
    for (auto& connInfo : _connections) {
        auto msg = std::make_shared<bt_messages::ToClientMessage>();
        msg->mutable_game_removed()->set_game_id(gameId);
        connInfo.second.connection->sendMessage(msg);
        if (connInfo.second.game == gameId) {
            connInfo.second.game = 0;
        }
    }
}

void GameServer::_sendStats() {
    auto msg = std::make_shared<bt_messages::ToClientMessage>();
    auto server_stats = msg->mutable_server_stats();
    server_stats->set_connected(_connections.size());
    server_stats->set_version(GIT_NAME);
    for (auto& conn : _connections) {
        conn.second.connection->sendMessage(msg);
    }
}

void GameServer::handleConnection() {
    auto conn = _server->nextPendingConnection();
    auto msgconn = new ToClientMessageSocket(conn, _nextConnectionId++, this);
    qInfo() << "New connection, id is" << msgconn->id();
    _connections[msgconn->id()] = {msgconn};

    connect(msgconn, &ToClientMessageSocket::messageRecieved, this, &GameServer::handleClientMessage);
    connect(msgconn, &ToClientMessageSocket::disconnected, this, &GameServer::handleDisconnection);

    qInfo() << "Sending game list";
    for (auto game : _games) {
        auto msg = std::make_shared<bt_messages::ToClientMessage>();
        auto game_updated = msg->mutable_game_updated();
        game_updated->set_game_id(game.first);
        game_updated->set_title(game.second->title());
        msgconn->sendMessage(msg);
    }
    _sendStats();
}

void GameServer::handleClientMessage(int id, std::shared_ptr<bt_messages::ToServerMessage> msg) {
    auto iter = _connections.find(id);
    if (iter == _connections.end()) {
        qWarning() << "Message from non-existent connection" << id;
        return;
    }
    auto& connInfo = iter->second;
    switch (msg->contents_case()) {
        case bt_messages::ToServerMessage::kExitGame: {
            if (!connInfo.game) {
                qWarning() << "Tried to exit a game when not in one";
            }
            else {
                qInfo() << "Exiting game";
                auto iter = _games.find(connInfo.game);
                if (iter == _games.end()) {
                    qWarning() << "Tried to leave a non-existent game" << connInfo.game;
                }
                else {
                    qInfo() << "Removing connection" << id << "to game" << connInfo.game;
                    iter->second->removeConnection(id);
                    connInfo.game = 0;
                }
            }
            break;
        }
        case bt_messages::ToServerMessage::kJoinGame: {
            auto iter = _games.find(msg->join_game().game_id());
            if (iter == _games.end()) {
                qWarning() << "Tried to join a non-existent game" << msg->join_game().game_id();
            }
            else {
                qInfo() << "Adding connection" << id << "to game" << msg->join_game().game_id();
                iter->second->addConnection(id, msg);
                connInfo.game = msg->join_game().game_id();
            }
            break;
        }
        case bt_messages::ToServerMessage::kCreateGame: {
            qInfo() << "Creating new game";
            auto& startingObjects = msg->create_game().starting_objects();

            auto gameId = addGame(std::vector<bt_messages::ToServerMessage_CreateGame_StartingObject>(startingObjects.begin(), startingObjects.end()), msg->create_game().title());
            auto msg = std::make_shared<bt_messages::ToClientMessage>();
            auto game_updated = msg->mutable_game_updated();
            game_updated->set_game_id(gameId);
            game_updated->set_title(_games[gameId]->title());
            for (auto& connInfo2 : _connections) {
                connInfo2.second.connection->sendMessage(msg);
            }
            break;
        }
        case bt_messages::ToServerMessage::kControlState: {
            if (connInfo.game > 0) {
                auto gameIter = _games.find(connInfo.game);
                if (gameIter == _games.end()) {
                    qWarning() << "Connection" << id << "is attached to non-existent game" << connInfo.game;
                    return;
                }
                gameIter->second->sendMessage(id, msg);
            }
            else {
                qWarning() << "Unrecognised message from connection" << id;
            }
            break;
        }
    }
}

void GameServer::handleGameMessage(int id, std::shared_ptr<bt_messages::ToClientMessage> msg) {
    auto iter = _connections.find(id);
    if (iter == _connections.end()) {
        qWarning() << "Message to non-existent connection" << id;
        return;
    }
    iter->second.connection->sendMessage(msg);
}

void GameServer::handleDisconnection(int id) {
    auto iter = _connections.find(id);
    if (iter != _connections.end()) {
        if (iter->second.game > 0) {
            auto gameIter = _games.find(iter->second.game);
            if (gameIter == _games.end()) {
                qWarning() << "Connection" << id << "is attached to non-existent game" << iter->second.game;
            }
            else {
                gameIter->second->removeConnection(id);
            }
        }
        iter->second.connection->deleteLater();
    }
    _connections.erase(id);
    _sendStats();
    if (!_connections.size()) {
        qInfo() << "Last client disconnected";
        emit lastClientDisconnected();
    }
}
