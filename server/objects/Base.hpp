#ifndef OBJECTS_BASE_HPP
#define OBJECTS_BASE_HPP

#include <memory>

#include "box2d/box2d.h"
#include "common/Constants.hpp"
#include "common/TcpMessageSocket.hpp"

enum class DamageType { IMPACT, PIERCING };

class Game;

class BaseObjectState {
    bool _dirty = true, _dead = false;
    b2Body* _body = nullptr;
    float _damageTaken = 0;
    int _side = 0;

   public:
    BaseObjectState();
    virtual ~BaseObjectState();
    virtual constants::ObjectType type() const = 0;
    virtual float maxHealth() const = 0;
    b2Body* body() const { return _body; }
    float health() const { return maxHealth() - _damageTaken; }
    bool dead() const { return _dead; }
    bool dirty() const { return _dirty; }
    int side() const { return _side; }
    void setSide(int side) { _side = side; }
    Message message() const;
    virtual void createBodies(b2World& world, b2BodyDef& bodyDef);
    virtual void prePhysics(Game* game);
    virtual void postPhysics(Game* game);
    virtual void handleMessage(const Message& msg);
    virtual void collision(BaseObjectState* other, float impulse);
    virtual void damage(float amount, DamageType type);
    virtual void destroy(Game* game);
    virtual std::pair<float, DamageType> impactDamage(float baseDamage);
    void die();
};

#endif  // OBJECTS_BASE_HPP