#ifndef OBJECTS_TANK_HPP
#define OBJECTS_TANK_HPP

#include "Base.hpp"

#include "actions/TankModule.hpp"

class TankState : public BaseObjectState {
    float _angle, _power;
    std::vector<std::unique_ptr<TankModule>> _actions;

   public:
    TankState();
    float maxHealth() const override;
    constants::ObjectType type() const override { return constants::ObjectType::TANK; }
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    void postPhysics(Game* game) override;
    void handleMessage(const Message& msg) override;
    void damage(float amount, DamageType type) override;
};

#endif  // OBJECTS_TANK_HPP
