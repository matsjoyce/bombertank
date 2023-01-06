#include "Game.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

constexpr float LOGIC_FPS = 20;
constexpr float PHYSICS_FPS = 100;

Game::Game(const std::map<int, ObjectTypeData>& objectTypeData, const std::map<int, TankModuleData>& tankModuleData) : _randomGen(std::random_device()()), _objectTypeData(objectTypeData), _tankModuleData(tankModuleData), _world(b2Vec2(0, 0)) { _world.SetContactListener(this); }

std::optional<std::pair<int, BaseObjectState*>> Game::addObject(constants::ObjectType type, b2Vec2 position, float rotation, b2Vec2 velocity) {
    if (!_objectTypeData.count(static_cast<int>(type))) {
        qWarning() << "Could not create object of unknown type" << static_cast<int>(type);
        return {};
    }
    auto objmove = ObjectStateRegister::createObject(_objectTypeData.at(static_cast<int>(type)).server.impl.toStdString(), static_cast<int>(type));
    if (!objmove) {
        qWarning() << "Could not create object of type" << static_cast<int>(type);
        return {};
    }
    auto id = _nextId++;
    auto& obj = _objects[id] = std::move(objmove);
    qInfo() << "Created object" << id << "of type" << static_cast<int>(type) << "at" << position.x << position.y;

    b2BodyDef bodyDef;
    bodyDef.position = position;
    bodyDef.angle = rotation;
    bodyDef.linearVelocity = velocity;

    obj->createBodies(this, _world, bodyDef);
    return {{id, obj.get()}};
}

void Game::PreSolve(b2Contact *contact, const b2Manifold* oldManifold) {
    auto bodyA = contact->GetFixtureA()->GetBody();
    auto bodyB = contact->GetFixtureB()->GetBody();
    auto objA = reinterpret_cast<BaseObjectState*>(bodyA->GetUserData().pointer);
    auto objB = reinterpret_cast<BaseObjectState*>(bodyB->GetUserData().pointer);
    if (objA->dead() || objB->dead()) {
        contact->SetEnabled(false);
    }
}

void Game::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
    auto bodyA = contact->GetFixtureA()->GetBody();
    auto bodyB = contact->GetFixtureB()->GetBody();
    auto objA = reinterpret_cast<BaseObjectState*>(bodyA->GetUserData().pointer);
    auto objB = reinterpret_cast<BaseObjectState*>(bodyB->GetUserData().pointer);
    if (!objA || !objB) {
        qWarning() << "Collision body has no object set!";
        return;
    }
    auto impulseFloat = 0.0f;
    for (auto i = 0; i < impulse->count; ++i) {
        impulseFloat += impulse->normalImpulses[i];
    }
    objA->collision(objB, impulseFloat);
    objB->collision(objA, impulseFloat);
}

void Game::mainloop() {
    qDebug() << "Game mainloop start";
    int physics_steps_per_logic_step = PHYSICS_FPS / LOGIC_FPS;

    while (_active) {
        QElapsedTimer timer;
        timer.start();
        QCoreApplication::processEvents();
        for (auto& m : _messages) {
            if (m.second->contents_case() == bt_messages::ToServerMessage::kControlState) {
                // TODO check permissions
                auto iter = _objects.find(m.second->control_state().object_id());
                if (iter != _objects.end()) {
                    iter->second->handleMessage(m.second->control_state());
                }
                else {
                    qWarning() << "Message to non-existant object" << m.second->control_state().object_id();
                }
            }
            else {
                qWarning() << "Game got unknown cmd" << m.second->contents_case();
            }
        }
        _messages.clear();

        for (auto& [id, obj] : _objects) {
            obj->prePhysics(this);
        }

        for (auto i = 0; i < physics_steps_per_logic_step; ++i) {
            _world.Step(1 / LOGIC_FPS / physics_steps_per_logic_step, 8, 3);
        }

        for (auto& [id, obj] : _objects) {
            obj->postPhysics(this);
        }

        for (auto iter = _objects.begin(); iter != _objects.end();) {
            if (iter->second->dead()) {
                auto& obj = iter->second;
                auto msg = std::make_shared<bt_messages::ToClientMessage>();
                obj->fillMessage(*msg->mutable_object_updated());
                msg->mutable_object_updated()->set_object_id(iter->first);
                if (_objToAttachedPlayer.count(iter->first)) {
                    auto playerId = _objToAttachedPlayer[iter->first];
                    _objToAttachedPlayer.erase(iter->first);
                    _playerToAttachedObj.erase(playerId);
                    emit playerAttachedObjectDied(playerId);
                }
                iter->second->destroy(this);
                for (auto c : _connections) {
                    emit sendMessage(c, msg);
                }
                _previousObjectMsg.erase(iter->first);
                iter = _objects.erase(iter);
            }
            else {
                ++iter;
            }
        }

        auto updateMsg = std::make_shared<bt_messages::ToClientMessage>();
        for (auto& [id, obj] : _objects) {
            updateMsg->clear_object_updated();
            obj->fillMessage(*updateMsg->mutable_object_updated());
            updateMsg->mutable_object_updated()->set_object_id(id);
            if (!_previousObjectMsg[id] || updateMsg->object_updated().SerializeAsString() != _previousObjectMsg[id]->object_updated().SerializeAsString()) {
                for (auto c : _connections) {
                    emit sendMessage(c, updateMsg);
                }
                if (_previousObjectMsg[id]) {
                    std::swap(_previousObjectMsg[id], updateMsg);
                }
                else {
                    _previousObjectMsg[id] = std::move(updateMsg);
                    updateMsg = std::make_shared<bt_messages::ToClientMessage>();
                }
            }
        }
        QCoreApplication::processEvents();
        QThread::msleep(1/LOGIC_FPS*1000);
        // qInfo() << timer.elapsed() << "ms in game loop";
    }
}

void Game::end() {
    qDebug() << "Ending game mainloop";
    _active = false;
}

void Game::addConnection(int id, std::shared_ptr<bt_messages::ToServerMessage> msg) {
    _connections.push_back(id);
    emit playerConnected(id, msg);
    for (auto [objId, objMsg] : _previousObjectMsg) {
        emit sendMessage(id, objMsg);
    }
}

void Game::removeConnection(int id) {
    _connections.erase(std::remove(_connections.begin(), _connections.end(), id), _connections.end());
}

void Game::recieveMessage(int id, std::shared_ptr<bt_messages::ToServerMessage> msg) { _messages.push_back(std::make_pair(id, msg)); }

BaseObjectState* Game::object(int id) {
    auto iter = _objects.find(id);
    if (iter == _objects.end()) {
        return nullptr;
    }
    return iter->second.get();
}

std::vector<int> Game::objectsOnSide(int side) const {
    std::vector<int> res;
    for (auto& obj : _objects) {
        if (obj.second->side() == side) {
            res.push_back(obj.first);
        }
    }
    return res;
}

std::vector<int> Game::objectsOfType(int type) const {
    std::vector<int> res;
    for (auto& obj : _objects) {
        if (obj.second->type() == type) {
            res.push_back(obj.first);
        }
    }
    return res;
}

void Game::attachPlayerToObject(int id, int objId) {
    if (_playerToAttachedObj.count(id)) {
        _objToAttachedPlayer.erase(_playerToAttachedObj[id]);
    }
    auto msg = std::make_shared<bt_messages::ToClientMessage>();
    msg->mutable_attach_to_object()->set_object_id(objId);
    emit sendMessage(id, msg);
    _objToAttachedPlayer[objId] = id;
    _playerToAttachedObj[id] = objId;
}

std::optional<int> Game::attachedObjectForPlayer(int id) const {
    if (_playerToAttachedObj.count(id)) {
        return {_playerToAttachedObj.at(id)};
    }
    return {};
}

float Game::timestep() const {
    return 1 / LOGIC_FPS;
}

const ServerOTD& Game::dataForType(int type) const {
    return _objectTypeData.at(type).server;
}
