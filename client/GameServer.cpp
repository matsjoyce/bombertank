#include "GameServer.hpp"

#include <QDebug>
#include <QFile>
#include <QQmlEngine>
#include <QTimer>
#include <algorithm>

#include "GameState.hpp"
#include "common/MsgpackUtils.hpp"

GameServer::GameServer(ToServerMessageSocket* msgconn, QObject* parent)
    : QObject(parent), _msgconn(msgconn), _listedGamesModel(new ListedGameModel(this)) {
    connect(_msgconn, &ToServerMessageSocket::messageRecieved, this, &GameServer::_handleMessage);
}

void GameServer::disconnect() {
    _msgconn->close();
    QObject::disconnect(_msgconn, &ToServerMessageSocket::messageRecieved, this, &GameServer::_handleMessage);
}


void GameServer::_handleMessage(int id, std::shared_ptr<bt_messages::ToClientMessage> msg) {
    switch (msg->contents_case()) {
        case bt_messages::ToClientMessage::kGameUpdated: {
            _listedGamesModel->_updateGame(msg->game_updated().game_id(), QString::fromStdString(msg->game_updated().title()));
            break;
        }
        case bt_messages::ToClientMessage::kGameRemoved: {
            _listedGamesModel->_removeGame(msg->game_removed().game_id());
            break;
        }
        case bt_messages::ToClientMessage::kServerStats: {
            _connectedCountProp.setValue(msg->server_stats().connected());
            _serverVersionProp.setValue(QString::fromStdString(msg->server_stats().version()));
            break;
        }
        default: {
            _gameState->handleMessage(id, msg);
            break;
        }
    }
}

GameState* GameServer::joinGame(int id, std::vector<int> modulesForSlots) {
    auto msg = std::make_shared<bt_messages::ToServerMessage>();
    auto join_game = msg->mutable_join_game();
    join_game->set_game_id(id);
    for (auto module : modulesForSlots) {
        join_game->add_tank_modules(module);
    }
    _msgconn->sendMessage(msg);

    _gameState = new GameState(this, id);
    connect(_gameState, &GameState::sendMessage, _msgconn, &ToServerMessageSocket::sendMessage);
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
    auto objs = extractVectorOfMap(oh.get().as<msgpack::type::variant>());

    auto msg = std::make_shared<bt_messages::ToServerMessage>();
    auto create_game = msg->mutable_create_game();
    create_game->set_title(title.toStdString());
    for (auto obj : objs) {
        auto starting_obj = create_game->add_starting_objects();
        starting_obj->set_type(obj.at("type").as_uint64_t());
        starting_obj->set_x(extractDouble(obj.at("x")));
        starting_obj->set_y(extractDouble(obj.at("y")));
        starting_obj->set_rotation(extractDouble(obj.at("rotation")));
        starting_obj->set_side(obj.at("side").as_uint64_t());
    }
    _msgconn->sendMessage(msg);
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
