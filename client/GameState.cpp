#include "GameState.hpp"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "GameServer.hpp"
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

std::shared_ptr<BaseObjectState>& GameState::_getOrCreateObject(const bt_messages::ToClientMessage_ObjectUpdated& msg) {
    auto iter = _objectStates.find(msg.object_id());
    if (iter == _objectStates.end()) {
        std::shared_ptr<BaseObjectState> objState;
        auto objType = static_cast<constants::ObjectType>(msg.type());
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
        iter = _objectStates.insert(std::make_pair(msg.object_id(), std::move(objState))).first;
    }
    return iter->second;
}

void GameState::handleMessage(int id, std::shared_ptr<bt_messages::ToClientMessage> msg) {
    switch (msg->contents_case()) {
        case bt_messages::ToClientMessage::kObjectUpdated: {
            _getOrCreateObject(msg->object_updated())->loadMessage(msg->object_updated());
            break;
        }
        case bt_messages::ToClientMessage::kAttachToObject: {
            emit attachToObject(msg->attach_to_object().object_id());
            break;
        }
        case bt_messages::ToClientMessage::kGameModeUpdate: {
            _livesLeftProp.setValue(msg->game_mode_update().lives_left());
            _livesTotalProp.setValue(msg->game_mode_update().lives_total());
            break;
        }
        case bt_messages::ToClientMessage::kDeadCanRejoin: {
            emit deadRejoin();
            break;
        }
        case bt_messages::ToClientMessage::kGameOver: {
            emit gameOver(msg->game_over().winner());
            break;
        }
        default: {
            qWarning() << "Unknown cmd sent to GameState" << msg->contents_case();
            break;
        }
    }
}

void GameState::setControlState(int objectId, TankControlState* controlState) {
    auto msg = std::make_shared<bt_messages::ToServerMessage>();
    auto& control_state = *msg->mutable_control_state();
    control_state.set_object_id(objectId);
    controlState->fillMessage(control_state);
    emit sendMessage(msg);
}

void GameState::exitGame() {
    auto msg = std::make_shared<bt_messages::ToServerMessage>();
    msg->mutable_exit_game();
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
    QJsonArray objs;
    int tankSide = 1;
    for (auto& obj : _objectStates) {
        objs.append(QJsonObject({
            {"type", static_cast<int>(obj.second->type())},
            {"x", obj.second->x()},
            {"y", obj.second->y()},
            {"rotation", obj.second->rotation()},
            {"side", obj.second->type() == static_cast<int>(constants::ObjectType::START_ZONE) ? tankSide++ : 0}
        }));
    }
    QJsonDocument doc;
    doc.setArray(objs);
    mapFile.write(doc.toJson(QJsonDocument::Compact));
}

EditorGameState* EditorGameState::load(QUrl fname, AppContext* context) {
    auto state = new EditorGameState(context);
    QFile mapFile(fname.toLocalFile());
    if (!mapFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open map file" << fname;
        return state;
    }
    QJsonDocument loadDoc(QJsonDocument::fromJson(mapFile.readAll()));

    for (auto item : loadDoc.array()) {
        auto obj = item.toObject();
        state->addObject(obj["type"].toInt(), obj["x"].toDouble(), obj["y"].toDouble(), obj["rotation"].toDouble());
    }

    QQmlEngine::setObjectOwnership(state, QQmlEngine::JavaScriptOwnership);
    return state;
}
