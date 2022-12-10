#include "GameServer.hpp"

#include <QDebug>
#include <QFile>
#include <QQmlEngine>
#include <QTimer>
#include <algorithm>

#include "GameState.hpp"

GameServer::GameServer(TcpMessageSocket* msgconn, QObject* parent)
    : QObject(parent), _msgconn(msgconn), _listedGamesModel(new ListedGameModel(this)) {
    connect(_msgconn, &TcpMessageSocket::messageRecieved, this, &GameServer::_handleMessage);
}

void GameServer::disconnect() {
    _msgconn->close();
    QObject::disconnect(_msgconn, &TcpMessageSocket::messageRecieved, this, &GameServer::_handleMessage);
}


void GameServer::_handleMessage(int id, const Message& msg) {
    if (msg.at("cmd").as_string() == "game_created") {
        _listedGamesModel->_updateGame(msg.at("id").as_uint64_t(), QString::fromStdString(msg.at("title").as_string()));
    }
    else if (msg.at("cmd").as_string() == "game_updated") {
        _listedGamesModel->_updateGame(msg.at("id").as_uint64_t(), QString::fromStdString(msg.at("title").as_string()));
    }
    else if (msg.at("cmd").as_string() == "game_removed") {
        _listedGamesModel->_removeGame(msg.at("id").as_uint64_t());
    }
    else if (msg.at("cmd").as_string() == "server_stats") {
        _connectedCountProp.setValue(msg.at("connected").as_uint64_t());
    }
    else if (_gameState) {
        _gameState->handleMessage(id, msg);
    }
}

GameState* GameServer::joinGame(int id, std::vector<int> modulesForSlots) {
    _msgconn->sendMessage({{"cmd", "join_game"}, {"id", id}, {"tank_modules", std::vector<msgpack::type::variant>(modulesForSlots.begin(), modulesForSlots.end())}});

    _gameState = new GameState(this, id);
    connect(_gameState, &GameState::sendMessage, _msgconn, &TcpMessageSocket::sendMessage);
    QQmlEngine::setObjectOwnership(_gameState, QQmlEngine::CppOwnership);
    return _gameState;
}

void GameServer::createGame(QUrl mapFilePath, QString title) {
    QFile mapFile(mapFilePath.toLocalFile());
    if (!mapFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open map file" << mapFilePath;
        return;
    }
    QByteArray mapData = mapFile.readAll();
    msgpack::object_handle oh = msgpack::unpack(mapData.constData(), mapData.size());
    auto objs = oh.get().as<std::vector<msgpack::type::variant>>();

    _msgconn->sendMessage({{"cmd", "create_game"}, {"starting_objects", objs}, {"title", title.toStdString()}});
}

int ListedGameModel::rowCount(const QModelIndex& parent) const { return _listedGames.size(); }

QVariant ListedGameModel::data(const QModelIndex& index, int role) const {
    if (index.isValid() && index.row() < _listedGames.size()) {
        auto& game = _listedGames[index.row()];
        switch (role) {
            case IdRole: {
                return {game.id};
            }
            case TitleRole: {
                return {game.title};
            }
        }
    }
    return QVariant();
}

QHash<int, QByteArray> ListedGameModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TitleRole] = "title";
    return roles;
}

void ListedGameModel::_removeGame(int id) {
    auto iter = std::lower_bound(_listedGames.begin(), _listedGames.end(), id,
                                 [](const ListedGame& game, int id) { return game.id < id; });
    if (iter == _listedGames.end()) {
        return;
    }
    auto pos = std::distance(_listedGames.begin(), iter);
    beginRemoveRows(QModelIndex(), pos, pos);
    _listedGames.erase(iter);
    endRemoveRows();
}

void ListedGameModel::_updateGame(int id, QString title) {
    auto iter = std::lower_bound(_listedGames.begin(), _listedGames.end(), id,
                                 [](const ListedGame& game, int id) { return game.id < id; });
    if (iter == _listedGames.end() || iter->id != id) {
        auto lg = ListedGame();
        lg.id = id;
        lg.title = title;
        auto pos = std::distance(_listedGames.begin(), iter);
        beginInsertRows(QModelIndex(), pos, pos);
        _listedGames.insert(iter, lg);
        endInsertRows();
    }
    else {
        iter->title = title;
        auto pos = std::distance(_listedGames.begin(), iter);
        auto idx = index(pos, 0);
        emit dataChanged(idx, idx);
    }
}
