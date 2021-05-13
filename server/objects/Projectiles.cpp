#include "Projectiles.hpp"

ShellState::ShellState(ObjectType type_) : BaseObjectState(type_, 2) {}

void ShellState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;

    BaseObjectState::createBodies(world, bodyDef);

    b2CircleShape circ;
    circ.m_radius = 0.25f;

    body->CreateFixture(&circ, 300.0);
}

void ShellState::prePhysics(Game* game) {
    --_selfDestruct;
    if (_selfDestruct < 0) {
        dead = true;
    }
}

std::pair<float, DamageType> ShellState::impactDamage(float baseDamage) {
    return {baseDamage * 100, DamageType::PIERCING};
};
