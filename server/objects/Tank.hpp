#ifndef OBJECTS_TANK_HPP
#define OBJECTS_TANK_HPP

#include "Base.hpp"

class TankState : public BaseObjectState {
    float _angle, _power;
    std::vector<bool> _actions;
    int _reload = 0;

   public:
    TankState();
    float maxHealth() const override;
    constants::ObjectType type() const override { return constants::ObjectType::TANK; }
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    void handleMessage(const Message& msg) override;
    void damage(float amount, DamageType type) override;
};

#endif  // OBJECTS_TANK_HPP
