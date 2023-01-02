#ifndef OBJECTS_CRATE_HPP
#define OBJECTS_CRATE_HPP

#include "Base.hpp"

class CrateState : public BaseObjectState {
    b2Body* _groundBody = nullptr;

   public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override;
    void createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) override;
};
REGISTER_STATE(CrateState)

class BombState : public CrateState {
public:
    using CrateState::CrateState;
    float maxHealth() const override;
    void destroy(Game * game) override;
    Hostility hostility() const override { return Hostility::VAGELY_HOSTILE; }
};
REGISTER_STATE(BombState)

class TimedBombState : public BombState {
    int _timer = 40;
public:
    using BombState::BombState;
    void prePhysics(Game* game) override;
};
REGISTER_STATE(TimedBombState)

class MineState : public BombState {
    int _timer = 40;
public:
    using BombState::BombState;
    void createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    void collision(BaseObjectState* other, float impulse) override;
};
REGISTER_STATE(MineState)

#endif  // OBJECTS_CRATE_HPP
