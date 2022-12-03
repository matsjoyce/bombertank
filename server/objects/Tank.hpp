#ifndef OBJECTS_TANK_HPP
#define OBJECTS_TANK_HPP

#include "Base.hpp"

#include "actions/TankModule.hpp"

class TankState : public BaseObjectState {
    float _turretAngle = 0, _targetTurretAngle = 0, _slewRate = 0.15;
    std::vector<std::unique_ptr<TankModule>> _actions;
    float _shield = 0;
    float _leftTrack = 0, _rightTrack = 0;

   public:
    TankState();
    float maxHealth() const override;
    float maxSpeed() const;
    float maxShield() const;
    constants::ObjectType type() const override { return constants::ObjectType::TANK; }
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    void postPhysics(Game* game) override;
    void handleMessage(const Message& msg) override;
    void damage(float amount, DamageType type) override;
    void addShield(float amount);
    Message message() const override;
    Hostility hostility() const override { return Hostility::HOSTILE; }
    float turretAngle() const { return _turretAngle; }
    b2Vec2 turretVector() const { return {std::cos(_turretAngle), std::sin(_turretAngle)}; }
};

#endif  // OBJECTS_TANK_HPP
