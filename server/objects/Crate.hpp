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
};

#endif  // OBJECTS_CRATE_HPP
