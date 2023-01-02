#ifndef OBJECTS_TURRET_HPP
#define OBJECTS_TURRET_HPP

#include "Base.hpp"

class LaserState;


class TurretState : public BaseObjectState {
    float _targettingRange = 50, _turretAngle = 0, _targetTurretAngle = 0, _slewRate = 0.15;

   protected:
    virtual void fire(float angle, Game* game) = 0;

   public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override;
    void createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    Message message() const override;
    Hostility hostility() const override { return Hostility::HOSTILE; }
};

class LaserTurretState : public TurretState {
    int _energy = 0;
    bool _armed = false;
    float _targetDist = 0;
    LaserState* _laser = nullptr;
   protected:
    void fire(float angle, Game* game) override;
   public:
    using TurretState::TurretState;
    void prePhysics(Game* game) override;
    void postPhysics(Game* game) override;
};
REGISTER_STATE(LaserTurretState)

class MachineGunTurretState : public TurretState {
    int _reload = 0;
   protected:
    void fire(float angle, Game* game) override;
   public:
    using TurretState::TurretState;
    void prePhysics(Game* game) override;
};
REGISTER_STATE(MachineGunTurretState)

#endif  // OBJECTS_TURRET_HPP
