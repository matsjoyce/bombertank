#include "GameServer.hpp"

#include <QDebug>
#include <QThread>

#include "Game.hpp"
#include "GameMode.hpp"
#include "common/MsgpackUtils.hpp"

GameHandler::GameHandler(GameServer* gs,
                         std::vector<std::map<msgpack::type::variant, msgpack::type::variant>> startingObjects)
    : QObject(gs) {
    auto thread = new QThread();
    connect(thread, &QThread::started, [=]() {
        auto game = new Game();
        qInfo() << "Loading objects" << startingObjects.size();

        for (auto& obj : startingObjects) {
            auto go = game->addObject(static_cast<constants::ObjectType>(obj.at("type").as_uint64_t()),
                                      {obj.at("x").as_double(), obj.at("y").as_double()}, 0, {0, 0});
            if (go) {
                go->second->setSide(obj.at("side").as_uint64_t());
            }
        }
        connect(game, &Game::sendMessage, gs, &GameServer::handleGameMessage);
        connect(this, &GameHandler::_addConnection, game, &Game::addConnection);
        connect(this, &GameHandler::_removeConnection, game, &Game::removeConnection);
        connect(this, &GameHandler::_sendMessage, game, &Game::recieveMessage, Qt::QueuedConnection);

        auto gameMode = new IndividualDeathMatch();
        gameMode->setGame(game);
        connect(game, &Game::destroyed, gameMode, &GameMode::deleteLater);

        game->mainloop();
        game->deleteLater();
        thread->quit();
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void GameHandler::addConnection(int id, Message msg) { emit _addConnection(id, msg); }

void GameHandler::removeConnection(int id) { emit _removeConnection(id); }

void GameHandler::sendMessage(int id, Message msg) { emit _sendMessage(id, msg); }

GameServer::GameServer(const QHostAddress& address, quint16 port) {
    _server = new QTcpServer(this);
    if (!_server->listen(address, port)) {
        qWarning() << "Failed to bind to server port";
        return;
    }
    qInfo() << "Server started, listening at" << _server->serverAddress() << ":" << _server->serverPort();

    connect(_server, &QTcpServer::newConnection, this, &GameServer::handleConnection);
}

int GameServer::addGame(const std::vector<std::map<msgpack::type::variant, msgpack::type::variant>>& startingObjects) {
    auto id = _nextGameId++;
    _games[id] = new GameHandler(this, startingObjects);
    return id;
}

void GameServer::handleConnection() {
    auto conn = _server->nextPendingConnection();
    auto msgconn = new TcpMessageSocket(conn, _nextConnectionId++, this);
    qInfo() << "New connection, id is" << msgconn->id();
    _connections[msgconn->id()] = {msgconn};

    connect(msgconn, &TcpMessageSocket::messageRecieved, this, &GameServer::handleClientMessage);
    connect(msgconn, &TcpMessageSocket::disconnected, this, &GameServer::handleDisconnection);

    qInfo() << "Sending game list";
    for (auto game : _games) {
        msgconn->sendMessage({{"cmd", "game_updated"}, {"id", game.first}});
    }
}

void GameServer::handleClientMessage(int id, Message msg) {
    auto iter = _connections.find(id);
    if (iter == _connections.end()) {
        qWarning() << "Message from non-existent connection" << id;
        return;
    }
    auto& connInfo = iter->second;
    if (msg["cmd"].as_string() == "exit_game") {
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
    }
    else if (connInfo.game > 0) {
        auto gameIter = _games.find(connInfo.game);
        if (gameIter == _games.end()) {
            qWarning() << "Connection" << id << "is attached to non-existent game" << connInfo.game;
            return;
        }
        gameIter->second->sendMessage(id, msg);
    }
    else if (msg["cmd"].as_string() == "join_game") {
        auto iter = _games.find(msg["id"].as_uint64_t());
        if (iter == _games.end()) {
            qWarning() << "Tried to join a non-existent game" << msg["id"].as_uint64_t();
        }
        else {
            qInfo() << "Adding connection" << id << "to game" << msg["id"].as_uint64_t();
            iter->second->addConnection(id, msg);
            connInfo.game = msg["id"].as_uint64_t();
        }
    }
    else if (msg["cmd"].as_string() == "create_game") {
        qInfo() << "Creating new game";
        auto gameId = addGame(extractVectorOfMap(msg["starting_objects"]));
        for (auto& connInfo2 : _connections) {
            connInfo2.second.connection->sendMessage({{"cmd", "game_created"}, {"id", gameId}});
        }
    }
    else {
        qWarning() << "Unrecognised message from connection" << id;
    }
}

void GameServer::handleGameMessage(int id, Message msg) {
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
    }
    _connections.erase(id);
    if (!_connections.size()) {
        qInfo() << "Last client disconnected";
        emit lastClientDisconnected();
    }
}
