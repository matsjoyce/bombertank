#include "GameState.hpp"

#include <QDebug>

#include "GameServer.hpp"

GameState::GameState(GameServer* server) : BaseGameState(server), _server(server) { qInfo() << "GameState started"; }

const std::map<int, std::unique_ptr<BaseObjectState>>& GameState::snapshot() const { return _objectStates; }

void GameState::handleMessage(int id, Message msg) {
    if (msg["cmd"].as_string() == "object") {
        auto id = msg["id"].as_uint64_t();
        auto iter = _objectStates.find(id);
        if (iter == _objectStates.end()) {
            iter = _objectStates.insert(std::make_pair(id, std::make_unique<BaseObjectState>())).first;
        }
        iter->second->loadMessage(msg);
    }
    else if (msg["cmd"].as_string() == "destroy_object") {
        _objectStates.erase(msg["id"].as_uint64_t());
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


const std::map<int, std::unique_ptr<BaseObjectState>>& EditorGameState::snapshot() const { return _objectStates; }

void EditorGameState::clear() {
    _objectStates.clear();
    _nextId = 1;
}

int EditorGameState::addObject(int type, float x, float y) {
    auto id = _nextId++;
    _objectStates[id] = std::make_unique<BaseObjectState>();
    _objectStates[id]->setFromEditor(static_cast<constants::ObjectType>(type), x, y);
    return id;
}

