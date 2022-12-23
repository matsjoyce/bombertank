#ifndef OBJECTS_CRATE_HPP
#define OBJECTS_CRATE_HPP

#include "Base.hpp"

class CrateState : public BaseObjectState {
    b2Body* _groundBody = nullptr;

   public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override;
    constants::ObjectType type() const override { return constants::ObjectType::CRATE; }
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
};

class BombState : public CrateState {
public:
    using CrateState::CrateState;
    float maxHealth() const override;
    constants::ObjectType type() const override { return constants::ObjectType::BOMB; }
    void destroy(Game * game) override;
    Hostility hostility() const override { return Hostility::VAGELY_HOSTILE; }
};

class TimedBombState : public BombState {
    int _timer = 40;
public:
    using BombState::BombState;
    constants::ObjectType type() const override { return constants::ObjectType::TIMED_BOMB; }
    void prePhysics(Game* game) override;
};

class MineState : public BombState {
    int _timer = 40;
public:
    using BombState::BombState;
    constants::ObjectType type() const override { return constants::ObjectType::MINE; }
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    void collision(BaseObjectState* other, float impulse) override;
};

#endif  // OBJECTS_CRATE_HPP
