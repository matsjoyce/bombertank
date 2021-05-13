#include "GameServer.hpp"

#include <QDebug>
#include <QQmlEngine>
#include <QTimer>
#include <algorithm>

#include "GameState.hpp"

GameServer::GameServer(TcpMessageSocket* msgconn, QObject* parent)
    : QObject(parent), _msgconn(msgconn), _listedGamesModel(new ListedGameModel(this)) {
    connect(_msgconn, &TcpMessageSocket::messageRecieved, this, &GameServer::_handleMessage);
}

void GameServer::_handleMessage(int id, const Message& msg) {
    if (msg.at("cmd").as_string() == "game_created") {
        _listedGamesModel->_updateGame(msg.at("id").as_uint64_t(), "New game");
    }
    if (msg.at("cmd").as_string() == "game_updated") {
        _listedGamesModel->_updateGame(msg.at("id").as_uint64_t(), "New game");
    }
    else if (_gameState) {
        _gameState->handleMessage(id, msg);
    }
}

GameState* GameServer::joinGame(int id) {
    _msgconn->sendMessage({{"cmd", "join_game"}, {"id", id}});

    _gameState = new GameState(this);
    connect(_gameState, &GameState::sendMessage, _msgconn, &TcpMessageSocket::sendMessage);
    QQmlEngine::setObjectOwnership(_gameState, QQmlEngine::CppOwnership);
    return _gameState;
}

void GameServer::createGame() { _msgconn->sendMessage({{"cmd", "create_game"}}); }

void ListedGame::setTitle(QString title) {
    if (_title != title) {
        _title = title;
        emit titleChanged(_title);
    }
}

int ListedGameModel::rowCount(const QModelIndex& parent) const { return _listedGames.size(); }

QVariant ListedGameModel::data(const QModelIndex& index, int role) const {
    if (index.isValid() && index.row() < _listedGames.size() && role == Qt::UserRole) {
        return QVariant::fromValue<QObject*>(_listedGames[index.row()]);
    }
    return QVariant();
}

QHash<int, QByteArray> ListedGameModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "listedGame";
    return roles;
}

void ListedGameModel::_removeGame(int id) {
    auto iter = std::lower_bound(_listedGames.begin(), _listedGames.end(), id,
                                 [](ListedGame* game, int id) { return game->id() < id; });
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
                                 [](ListedGame* game, int id) { return game->id() < id; });
    if (iter == _listedGames.end() || (*iter)->id() != id) {
        auto lg = new ListedGame(id, this);
        lg->setTitle(title);
        auto pos = std::distance(_listedGames.begin(), iter);
        beginInsertRows(QModelIndex(), pos, pos);
        _listedGames.insert(iter, lg);
        endInsertRows();
    }
    else {
        auto pos = std::distance(_listedGames.begin(), iter);
        auto idx = index(pos, 0);
        emit dataChanged(idx, idx);
    }
}
