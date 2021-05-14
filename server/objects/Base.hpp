#ifndef OBJECTS_BASE_HPP
#define OBJECTS_BASE_HPP

#include <memory>

#include "box2d/box2d.h"
#include "common/Constants.hpp"
#include "common/TcpMessageSocket.hpp"

enum class DamageType { IMPACT, PIERCING };

class Game;

struct BaseObjectState {
    constants::ObjectType type;
    bool dirty = true, dead = false;
    b2Body* body = nullptr;
    float health, maxHealth;

    BaseObjectState(constants::ObjectType type_, float maxHealth_) : type(type_), health(maxHealth_), maxHealth(maxHealth_) {}
    virtual ~BaseObjectState();
    Message message() const;
    virtual void createBodies(b2World& world, b2BodyDef& bodyDef);
    virtual void prePhysics(Game* game);
    virtual void postPhysics(Game* game);
    virtual void handleMessage(const Message& msg);
    virtual void collision(BaseObjectState* other, float impulse);
    virtual void damage(float amount, DamageType type);
    virtual void destroy(Game* game);
    virtual std::pair<float, DamageType> impactDamage(float baseDamage);
};

#endif  // OBJECTS_BASE_HPP