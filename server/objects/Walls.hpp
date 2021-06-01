#ifndef OBJECTS_WALLS_HPP
#define OBJECTS_WALLS_HPP

#include "Base.hpp"

class AbstractWallState : public BaseObjectState {
   public:
    using BaseObjectState::BaseObjectState;
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
};

class WallState : public AbstractWallState {
   public:
    using AbstractWallState::AbstractWallState;
    float maxHealth() const override;
    constants::ObjectType type() const override { return constants::ObjectType::WALL; }
};

class IndestructableWallState : public AbstractWallState {
   public:
    using AbstractWallState::AbstractWallState;
    float maxHealth() const override;
    constants::ObjectType type() const override { return constants::ObjectType::INDESTRUCTABLE_WALL; }
    void damage(float amount, DamageType type) override {}
};

#endif  // OBJECTS_WALLS_HPP