#include "Game.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <functional>

#include "objects/Objects.hpp"

constexpr float LOGIC_FPS = 20;
constexpr float PHYSICS_FPS = 100;

Game::Game() : _randomGen(std::random_device()()), _world(b2Vec2(0, 0)) { _world.SetContactListener(this); }

std::optional<std::pair<int, BaseObjectState*>> Game::addObject(constants::ObjectType type, b2Vec2 position, float rotation, b2Vec2 velocity) {
    auto objmove = createObjectFromType(type);
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

    obj->createBodies(_world, bodyDef);
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

    while (true) {
        QElapsedTimer timer;
        timer.start();
        QCoreApplication::processEvents();
        for (auto& m : _messages) {
            if (m.second["cmd"].as_string() == "control_state") {
                // TODO check permissions
                auto iter = _objects.find(m.second["id"].as_uint64_t());
                if (iter != _objects.end()) {
                    iter->second->handleMessage(m.second);
                }
                else {
                    qWarning() << "Message to non-existant object" << m.second["id"].as_uint64_t();
                }
            }
            else {
                qWarning() << "Game got unknown cmd" << m.second["cmd"].as_string().c_str();
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

        for (auto iter = _objects.begin(); iter != _objects.end(); ++iter) {
            if (iter->second->dead()) {
                auto& obj = iter->second;
                auto msg = obj->message();
                msg["cmd"] = "destroy_object";
                msg["id"] = iter->first;
                iter->second->destroy(this);
                for (auto c : _connections) {
                    emit sendMessage(c, msg);
                }
                iter = _objects.erase(iter);
            }
        }

        for (auto& [id, obj] : _objects) {
            if (obj->dirty()) {
                // obj->state->dirty = false;
                auto msg = obj->message();
                msg["cmd"] = "object";
                msg["id"] = id;
                for (auto c : _connections) {
                    emit sendMessage(c, msg);
                }
            }
        }
        QCoreApplication::processEvents();
        QThread::msleep(1/LOGIC_FPS*1000);
        // qInfo() << timer.elapsed() << "ms in game loop";
    }
}

void Game::addConnection(int id, Message msg) {
    _connections.push_back(id);
    emit playerConnected(id, msg);
}

void Game::removeConnection(int id) {
    _connections.erase(std::remove(_connections.begin(), _connections.end(), id), _connections.end());
}

void Game::recieveMessage(int id, Message msg) { _messages.push_back(std::make_pair(id, msg)); }

BaseObjectState* Game::object(int id) {
    auto iter = _objects.find(id);
    if (iter == _objects.end()) {
        return nullptr;
    }
    return iter->second.get();
}

std::vector<int> Game::objectsOnSide(int side) {
    std::vector<int> res;
    for (auto& obj : _objects) {
        if (obj.second->side() == side) {
            res.push_back(obj.first);
        }
    }
    return res;
}

std::vector<int> Game::objectsOfType(constants::ObjectType type) {
    std::vector<int> res;
    for (auto& obj : _objects) {
        if (obj.second->type() == type) {
            res.push_back(obj.first);
        }
    }
    return res;
}

void Game::attachPlayerToObject(int id, int objId) {
    emit sendMessage(id, {{"cmd", "attach"}, {"id", objId}});
}
