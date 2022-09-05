#include "GameState.hpp"

#include <QDebug>
#include <QFile>

#include "GameServer.hpp"
#include "common/MsgpackUtils.hpp"
#include "objects/TankState.hpp"

GameState::GameState(GameServer* server) : BaseGameState(server), _server(server) { qInfo() << "GameState started"; }

const std::map<int, std::shared_ptr<BaseObjectState>>& GameState::snapshot() const { return _objectStates; }

void GameState::handleMessage(int id, Message msg) {
    if (msg["cmd"].as_string() == "object") {
        auto id = msg["id"].as_uint64_t();
        auto iter = _objectStates.find(id);
        if (iter == _objectStates.end()) {
            std::shared_ptr<BaseObjectState> objState;
            qInfo() << "hai" << msg["type"].as_uint64_t() << (static_cast<constants::ObjectType>(msg["type"].as_uint64_t()) == constants::ObjectType::TANK);
            if (static_cast<constants::ObjectType>(msg["type"].as_uint64_t()) == constants::ObjectType::TANK) {
                qInfo() << "Tank";
                objState = std::make_shared<TankState>();
                qInfo() << objState.get();
            }
            else {
                objState = std::make_shared<BaseObjectState>();
            }
            iter = _objectStates.insert(std::make_pair(id, std::move(objState))).first;
        }
        iter->second->loadMessage(msg);
    }
    else if (msg["cmd"].as_string() == "destroy_object") {
        auto iter = _objectStates.find(msg["id"].as_uint64_t());
        if (iter != _objectStates.end()) {
            iter->second->loadMessage(msg);
            iter->second->setDestroyed(true);
            _objectStates.erase(iter);
        }
    }
    else if (msg["cmd"].as_string() == "attach") {
        emit attachToObject(msg["id"].as_uint64_t());
    }
    else {
        qWarning() << "Unknown cmd sent to GameState" << msg["cmd"].as_string().c_str();
    }
}

void GameState::setControlState(int objectId, TankControlState* controlState) {
    auto msg = controlState->message();
    msg["cmd"] = "control_state";
    msg["id"] = objectId;
    emit sendMessage(msg);
}

void GameState::exitGame() {
    Message msg = {{"cmd", "exit_game"}};
    emit sendMessage(msg);
}


const std::map<int, std::shared_ptr<BaseObjectState>>& EditorGameState::snapshot() const { return _objectStates; }

void EditorGameState::clear() {
    _objectStates.clear();
    _nextId = 1;
}

int EditorGameState::addObject(int type, float x, float y) {
    auto id = _nextId++;
    _objectStates[id] = std::make_shared<BaseObjectState>();
    _objectStates[id]->setFromEditor(static_cast<constants::ObjectType>(type), x, y);
    return id;
}

void EditorGameState::save(QUrl fname) const {
    QFile mapFile(fname.toLocalFile());
    if (!mapFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open map file" << fname;
        return;
    }
    std::vector<std::map<std::string, msgpack::type::variant>> objs;
    int tankSide = 1;
    for (auto& obj : _objectStates) {
        objs.emplace_back(std::map<std::string, msgpack::type::variant>{
            {"type", static_cast<int>(obj.second->type())},
            {"x", obj.second->x()},
            {"y", obj.second->y()},
            {"rotation", obj.second->rotation()},
            {"side", obj.second->type() == constants::ObjectType::TANK ? tankSide++ : 0}
        });
    }
    msgpack::pack(mapFile, objs);
}

EditorGameState* EditorGameState::load(QUrl fname) {
    auto state = new EditorGameState();
    QFile mapFile(fname.toLocalFile());
    if (!mapFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open map file" << fname;
        return state;
    }
    QByteArray mapData = mapFile.readAll();
    msgpack::object_handle oh = msgpack::unpack(mapData.constData(), mapData.size());
    auto objs = extractVectorOfMap(oh.get().as<msgpack::type::variant>());

    for (auto& obj : objs) {
        state->addObject(obj["type"].as_uint64_t(), obj["x"].as_double(), obj["y"].as_double());
    }

    return state;
}

