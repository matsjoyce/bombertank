#ifndef OBJECTS_PROJECTILES_HPP
#define OBJECTS_PROJECTILES_HPP

#include "Base.hpp"

class ShellState : public BaseObjectState {
    int _selfDestruct = 20;

protected:
    virtual float _bodyRadius() { return 0.25; }
    virtual int _category() { return SHELL_CATEGORY; }
    virtual int _collisionMask() { return 0xffff & ~SHELL_CATEGORY; }

   public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override;
    void createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    std::pair<float, DamageType> impactDamage(float baseDamage) override;
    void collision(BaseObjectState * other, float impulse) override;
};
REGISTER_STATE(ShellState)

class MGShellState : public ShellState {
protected:
    float _bodyRadius() override { return 0.125; }
public:
    using ShellState::ShellState;
    void collision(BaseObjectState * other, float impulse) override;
};
REGISTER_STATE(MGShellState)

class RocketState : public ShellState {
protected:
    float _bodyRadius() override { return 0.5; }
    int _category() override { return ROCKET_CATEGORY; }
    int _collisionMask() override { return 0xffff; }
public:
    using ShellState::ShellState;
    void prePhysics(Game* game) override;
    void destroy(Game * game) override;
    void collision(BaseObjectState * other, float impulse) override;
    Hostility hostility() const override { return Hostility::VAGELY_HOSTILE; }
};
REGISTER_STATE(RocketState)

class HomingRocketState : public RocketState {
public:
    using RocketState::RocketState;
    void prePhysics(Game* game) override;
};
REGISTER_STATE(HomingRocketState)

class ExplosionState : public BaseObjectState {
    float _damageMultiplier = 1;
public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override { return 0; }
    void prePhysics(Game* game) override;
    void setDamageMultiplier(float mul) { _damageMultiplier = mul; }
};
REGISTER_STATE(ExplosionState)

class StunWaveState : public BaseObjectState {
public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override { return 0; }
    void prePhysics(Game* game) override;
};
REGISTER_STATE(StunWaveState)

class LaserState : public BaseObjectState {
    float _length = 0, _maxLength = 0;
public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override { return 0; }
    void prePhysics(Game* game) override;
    Message message() const override;
    void setMaxLength(float maxLength) { _maxLength = maxLength; }
};
REGISTER_STATE(LaserState)

#endif  // OBJECTS_PROJECTILES_HPP
