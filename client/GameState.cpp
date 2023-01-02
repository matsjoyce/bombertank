#include "GameState.hpp"

#include <QDebug>
#include <QFile>

#include "GameServer.hpp"
#include "common/MsgpackUtils.hpp"
#include "objects/TankState.hpp"
#include "objects/TurretState.hpp"
#include "AppContext.hpp"

GameState::GameState(GameServer* server, int id) : BaseGameState(server), _server(server), _id(id) { qInfo() << "GameState started"; }

const std::map<int, std::shared_ptr<BaseObjectState>>& GameState::snapshot() const { return _objectStates; }

void GameState::cleanup() {
    for (auto iter = _objectStates.begin(); iter != _objectStates.end();) {
        if (iter->second->destroyed()) {
            iter = _objectStates.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

std::shared_ptr<BaseObjectState>& GameState::_getOrCreateObject(const Message& msg) {
    auto id = msg.at("id").as_uint64_t();
    auto iter = _objectStates.find(id);
    if (iter == _objectStates.end()) {
        std::shared_ptr<BaseObjectState> objState;
        auto objType = static_cast<constants::ObjectType>(msg.at("type").as_uint64_t());
        if (objType == constants::ObjectType::TANK) {
            objState = std::make_shared<TankState>();
        }
        else if (objType == constants::ObjectType::LASER_TURRET || objType == constants::ObjectType::MG_TURRET) {
            objState = std::make_shared<TurretState>();
        }
        else if (objType == constants::ObjectType::LASER) {
            objState = std::make_shared<LaserState>();
        }
        else {
            objState = std::make_shared<BaseObjectState>();
        }
        iter = _objectStates.insert(std::make_pair(id, std::move(objState))).first;
    }
    return iter->second;
}

void GameState::handleMessage(int id, Message msg) {
    if (msg["cmd"].as_string() == "object") {
        _getOrCreateObject(msg)->loadMessage(msg);
    }
    else if (msg["cmd"].as_string() == "destroy_object") {
        auto obj = _getOrCreateObject(msg);
        obj->loadMessage(msg);
        obj->setDestroyed(true);
    }
    else if (msg["cmd"].as_string() == "attach") {
        emit attachToObject(msg["id"].as_uint64_t());
    }
    else if (msg["cmd"].as_string() == "livesLeft") {
        _livesLeftProp.setValue(msg["left"].as_uint64_t());
        _livesTotalProp.setValue(msg["total"].as_uint64_t());
    }
    else if (msg["cmd"].as_string() == "deadRejoin") {
        emit deadRejoin();
    }
    else if (msg["cmd"].as_string() == "gameOver") {
        emit gameOver(msg["winner"].as_bool());
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

EditorGameState::EditorGameState(AppContext* context) {
    _contextProp.setValue(context);
    _contextNotifier = _contextProp.addNotifier([=](){ clear(); });
}

AppContext* EditorGameState::context() const {
    return _contextProp.value();
}

void EditorGameState::setContext(AppContext* context) {
    _contextProp.setValue(context);
}

QBindable<AppContext*> EditorGameState::bindableContext() const {
    return &_contextProp;
}

const std::map<int, std::shared_ptr<BaseObjectState>>& EditorGameState::snapshot() const { return _objectStates; }

void EditorGameState::clear() {
    for (auto& [_, treeData] : _objectTreeData) {
        _queryTree.DestroyProxy(treeData.proxyId);
    }
    _objectTreeData.clear();
    _objectStates.clear();
    _nextId = 1;
}

constexpr float AABB_REDUCTION = 0.05; // To prevent side-by-side objects removing each other

int EditorGameState::addObject(int type, float x, float y, float rotation) {
    if (!context()) return -1;

    auto& objectDatas = context()->objectTypeDatas();
    if (!objectDatas.count(type)) {
        qWarning() << "Could not load type" << type;
        return -1;
    }
    auto bounds = context()->objectTypeData(type).client.editorBounds;
    if (bounds.isEmpty()) {
        qWarning() << "Type" << type << "used in the editor has empty bounds, this will mess with object removal";
    }
    b2Vec2 trCorner{static_cast<float>(bounds.width() / 2 - AABB_REDUCTION), static_cast<float>(bounds.height() / 2 - AABB_REDUCTION)};
    b2Vec2 brCorner{trCorner.x, -trCorner.y};
    b2Rot rot(rotation);
    trCorner = b2Mul(rot, trCorner);
    brCorner = b2Mul(rot, brCorner);
    b2Vec2 center{x, y};

    b2AABB aabb{
        center + b2Min(b2Min(trCorner, brCorner), b2Min(-trCorner, -brCorner)),
        center + b2Max(b2Max(trCorner, brCorner), b2Max(-trCorner, -brCorner))
    };
    removeObjects(aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y);

    auto id = _nextId++;
    _objectStates[id] = std::make_shared<BaseObjectState>();
    _objectStates[id]->setFromEditor(static_cast<constants::ObjectType>(type), x, y, rotation);
    _objectTreeData.emplace(id, ObjectTreeData{id, aabb, 0});
    _objectTreeData[id].proxyId = _queryTree.CreateProxy(aabb, &_objectTreeData[id]);
    return id;
}

struct RemoveObjectsCallback {
    b2DynamicTree& tree;
    std::map<int, std::shared_ptr<BaseObjectState>>& objectStates;
    b2AABB& queryAABB;
    std::set<int> hits;

    bool QueryCallback(int32 nodeId) {
        auto treeData = static_cast<EditorGameState::ObjectTreeData*>(tree.GetUserData(nodeId));
        if (b2TestOverlap(treeData->realAABB, queryAABB)) {
            hits.insert(treeData->id);
        }
        return true;
    }
};

void EditorGameState::removeObjects(float x1, float y1, float x2, float y2) {
    b2AABB aabb = {{x1, y1}, {x2, y2}};
    RemoveObjectsCallback callback{_queryTree, _objectStates, aabb, {}};
    _queryTree.Query(&callback, aabb);
    for (auto id : callback.hits) {
        _removeObject(id);
    }
}

void EditorGameState::_removeObject(int id) {
    _queryTree.DestroyProxy(_objectTreeData[id].proxyId);
    _objectTreeData.erase(id);
    _objectStates.erase(id);
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
            {"side", obj.second->type() == static_cast<int>(constants::ObjectType::START_ZONE) ? tankSide++ : 0}
        });
    }
    msgpack::pack(mapFile, objs);
}

EditorGameState* EditorGameState::load(QUrl fname, AppContext* context) {
    auto state = new EditorGameState(context);
    QFile mapFile(fname.toLocalFile());
    if (!mapFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open map file" << fname;
        return state;
    }
    QByteArray mapData = mapFile.readAll();
    msgpack::object_handle oh = msgpack::unpack(mapData.constData(), mapData.size());
    auto objs = extractVectorOfMap(oh.get().as<msgpack::type::variant>());

    for (auto& obj : objs) {
        state->addObject(obj["type"].as_uint64_t(), extractDouble(obj["x"]), extractDouble(obj["y"]), extractDouble(obj["rotation"]));
    }

    QQmlEngine::setObjectOwnership(state, QQmlEngine::JavaScriptOwnership);
    return state;
}
