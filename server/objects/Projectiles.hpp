#ifndef OBJECTS_PROJECTILES_HPP
#define OBJECTS_PROJECTILES_HPP

#include "Base.hpp"

class ShellState : public BaseObjectState {
    int _selfDestruct = 20;

protected:
    float _bodyRadius() { return 0.25; }

   public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override;
    constants::ObjectType type() const override { return constants::ObjectType::SHELL; }
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    std::pair<float, DamageType> impactDamage(float baseDamage) override;
    void collision(BaseObjectState * other, float impulse) override;
};

class MGShellState : public ShellState {
protected:
    float _bodyRadius() { return 0.125; }
public:
    using ShellState::ShellState;
    constants::ObjectType type() const override { return constants::ObjectType::MG_SHELL; }
    void collision(BaseObjectState * other, float impulse) override;
};

class RocketState : public ShellState {
protected:
    float _bodyRadius() { return 0.5; }
public:
    using ShellState::ShellState;
    constants::ObjectType type() const override { return constants::ObjectType::ROCKET; }
    void prePhysics(Game* game) override;
    void destroy(Game * game) override;
};

class ExplosionState : public BaseObjectState {
public:
    float maxHealth() const override { return 0; }
    constants::ObjectType type() const override { return constants::ObjectType::EXPLOSION; }
    void prePhysics(Game* game) override;
};

class LaserState : public BaseObjectState {
    float _length = 0, _maxLength = 0;
public:
    float maxHealth() const override { return 0; }
    constants::ObjectType type() const override { return constants::ObjectType::LASER; }
    void prePhysics(Game* game) override;
    Message message() const override;
    void setMaxLength(float maxLength) { _maxLength = maxLength; }
};

#endif  // OBJECTS_PROJECTILES_HPP
