#ifndef OBJECTS_PROJECTILES_HPP
#define OBJECTS_PROJECTILES_HPP

#include "Base.hpp"

class ShellState : public BaseObjectState {
    int _selfDestruct = 20;

   public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override;
    constants::ObjectType type() const override { return constants::ObjectType::SHELL; }
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    std::pair<float, DamageType> impactDamage(float baseDamage) override;
};

#endif  // OBJECTS_PROJECTILES_HPP