#ifndef OBJECTS_TURRET_HPP
#define OBJECTS_TURRET_HPP

#include "Base.hpp"


class TurretState : public BaseObjectState {
    float _targettingRange = 50, _turretAngle = 0, _targetTurretAngle = 0, _slewRate = 0.15;
    int _reload = 0;

   public:
    TurretState();
    float maxHealth() const override;
    constants::ObjectType type() const override { return constants::ObjectType::LASER_TURRET; }
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    Message message() const override;
};

#endif  // OBJECTS_TURRET_HPP
