#ifndef OBJECTS_WALLS_HPP
#define OBJECTS_WALLS_HPP

#include "Base.hpp"

class AbstractWallState : public BaseObjectState {
   public:
    using BaseObjectState::BaseObjectState;
    void createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) override;
};

class WallState : public AbstractWallState {
   public:
    using AbstractWallState::AbstractWallState;
    float maxHealth() const override;
};
REGISTER_STATE(WallState)

class IndestructableWallState : public AbstractWallState {
   public:
    using AbstractWallState::AbstractWallState;
    float maxHealth() const override;
    void damage(float amount, DamageType type) override {}
};
REGISTER_STATE(IndestructableWallState)

#endif  // OBJECTS_WALLS_HPP
