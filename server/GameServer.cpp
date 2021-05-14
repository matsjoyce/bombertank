#include "GameServer.hpp"

#include <QDebug>
#include <QThread>

#include "Game.hpp"

GameHandler::GameHandler(GameServer* gs,
                         std::vector<std::map<msgpack::type::variant, msgpack::type::variant>> startingObjects)
    : QObject(gs) {
    auto thread = new QThread();
    connect(thread, &QThread::started, [=]() {
        auto game = new Game();
        qInfo() << "Loading objects" << startingObjects.size();

        for (auto& obj : startingObjects) {
            game->addObject(static_cast<constants::ObjectType>(obj.at("type").as_uint64_t()),
                            {obj.at("x").as_double(), obj.at("y").as_double()}, 0, {});
        }
        connect(game, &Game::sendMessage, gs, &GameServer::handleGameMessage);
        connect(this, &GameHandler::_addConnection, game, &Game::addConnection);
        connect(this, &GameHandler::_removeConnection, game, &Game::removeConnection);
        connect(this, &GameHandler::_sendMessage, game, &Game::recieveMessage, Qt::QueuedConnection);
        game->mainloop();
        game->deleteLater();
        thread->quit();
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void GameHandler::addConnection(int id) { emit _addConnection(id); }

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

std::vector<std::map<msgpack::type::variant, msgpack::type::variant>> extractVectorOfMap(msgpack::type::variant obj) {
    std::vector<std::map<msgpack::type::variant, msgpack::type::variant>> res;
    auto vec = obj.as_vector();
    std::transform(vec.begin(), vec.end(), std::back_inserter(res), [](auto& o) {
        std::map<msgpack::type::variant, msgpack::type::variant> m;
        for (auto& p : o.as_multimap()) {
            m.insert(p);
        }
        return m;
    });
    return res;
}

void GameServer::handleClientMessage(int id, Message msg) {
    auto iter = _connections.find(id);
    if (iter == _connections.end()) {
        qWarning() << "Message from non-existant connection" << id;
        return;
    }
    auto& connInfo = iter->second;
    if (connInfo.game > 0) {
        auto gameIter = _games.find(connInfo.game);
        if (gameIter == _games.end()) {
            qWarning() << "Connection" << id << "is attached to non-existant game" << connInfo.game;
            return;
        }
        gameIter->second->sendMessage(id, msg);
    }
    else if (msg["cmd"].as_string() == "join_game") {
        auto iter = _games.find(msg["id"].as_uint64_t());
        if (iter == _games.end()) {
            qWarning() << "Tried to join a non-existant game" << msg["id"].as_uint64_t();
        }
        else {
            qInfo() << "Adding connection" << id << "to game" << msg["id"].as_uint64_t();
            iter->second->addConnection(id);
            connInfo.game = msg["id"].as_uint64_t();
        }
    }
    else if (msg["cmd"].as_string() == "create_game") {
        qInfo() << "Creating new game";
        auto gameId = addGame(extractVectorOfMap(msg["starting_objects"]));
        connInfo.connection->sendMessage({{"cmd", "game_created"}, {"id", gameId}});
    }
    else {
        qWarning() << "Unrecognised message from connection" << id;
    }
}

void GameServer::handleGameMessage(int id, Message msg) {
    auto iter = _connections.find(id);
    if (iter == _connections.end()) {
        qWarning() << "Message to non-existant connection" << id;
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
                qWarning() << "Connection" << id << "is attached to non-existant game" << iter->second.game;
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
