#include "Projectiles.hpp"

const float IMPULSE_TO_DAMAGE = 1000.0f;

float ShellState::maxHealth() const { return 2; }

void ShellState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;

    BaseObjectState::createBodies(world, bodyDef);

    b2CircleShape circ;
    circ.m_radius = _bodyRadius();

    body()->CreateFixture(&circ, 300.0);
}

void ShellState::prePhysics(Game* game) {
    --_selfDestruct;
    if (_selfDestruct < 0) {
        die();
    }
}

std::pair<float, DamageType> ShellState::impactDamage(float baseDamage) {
    return {baseDamage * 100, DamageType::PIERCING};
}


void ShellState::collision(BaseObjectState* other, float impulse) {
    other->damage(impulse / IMPULSE_TO_DAMAGE * 3, DamageType::PIERCING);
    die();
}


void MGShellState::collision(BaseObjectState* other, float impulse) {
    other->damage(impulse / IMPULSE_TO_DAMAGE, DamageType::PIERCING);
    die();
}


void RocketState::prePhysics(Game* game) {
    ShellState::prePhysics(game);
    body()->ApplyLinearImpulseToCenter(body()->GetMass() * 10 * body()->GetWorldVector({1, 0}), true);
}
