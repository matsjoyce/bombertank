#include "Projectiles.hpp"
#include "../Game.hpp"
#include "Utils.hpp"
#include "Queries.hpp"

const float IMPULSE_TO_DAMAGE = 300.0f;

float ShellState::maxHealth() const { return 2; }

void ShellState::createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;

    BaseObjectState::createBodies(game, world, bodyDef);

    b2CircleShape circ;
    circ.m_radius = _bodyRadius();
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circ;
    fixtureDef.density = 100.0;
    fixtureDef.filter.categoryBits = _category();
    fixtureDef.filter.maskBits = _collisionMask();

    body()->CreateFixture(&fixtureDef);
}

void ShellState::prePhysics(Game* game) {
    BaseObjectState::prePhysics(game);
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

void RocketState::destroy(Game* game) {
    if (!stunned()) {
        game->addObject(constants::ObjectType::EXPLOSION, body()->GetPosition(), 0, {0, 0});
    }
    ShellState::destroy(game);
}

void RocketState::collision(BaseObjectState* other, float impulse) {
    die();
}

void HomingRocketState::prePhysics(Game* game) {
    RocketState::prePhysics(game);
    if (stunned()) {
        return;
    }
    // Weathercock
    auto velocity = body()->GetLinearVelocityFromLocalPoint({0, 0});
    auto velocityAngle = std::atan2(velocity.y, velocity.x);
    body()->SetTransform(body()->GetPosition(), velocityAngle);

    // Find targets in search cone
    auto center = body()->GetPosition();
    auto forward = body()->GetWorldVector({1, 0});
    auto prioritisedTargets = getPrioritsedTargets(queryObjectsInIsoscelesTriangle(game, center, forward, 50, M_PI/8), side(), center);
    if (prioritisedTargets.size()) {
        auto targetVector = (prioritisedTargets.front()->body()->GetPosition() - center);
        bool goLeft = b2Cross(targetVector, forward) > 0;
        body()->ApplyLinearImpulseToCenter(body()->GetMass() * 8 * (goLeft ? -1 : 1) * body()->GetWorldVector({0, 1}), true);
    }
}

void ExplosionState::prePhysics(Game* game) {
    BaseObjectState::prePhysics(game);
    auto center = body()->GetPosition();
    const int numRays = 50;
    const float radius = 15;
    for (int i = 0; i < numRays; i++) {
        float angle =  M_PI * 2 * i / numRays;
        b2Vec2 rayDir(std::cos(angle), std::sin(angle));
        b2Vec2 rayEnd = center + radius * rayDir;
        for (auto& obj : raycastAllObjects(game, center, rayEnd)) {
            obj->damage(4 * _damageMultiplier, DamageType::IMPACT);
        }
    }
    die();
}

void StunWaveState::prePhysics(Game* game) {
    BaseObjectState::prePhysics(game);
    for (auto obj : queryObjectsInCircle(game, body()->GetPosition(), 30)) {
        obj->stun(75);
    }
    die();
}

void LaserState::prePhysics(Game* game) {
    BaseObjectState::prePhysics(game);
    _length = _maxLength;
    if (_length) {
        auto center = body()->GetPosition();
        b2Vec2 rayDir(std::cos(body()->GetAngle()), std::sin(body()->GetAngle()));
        b2Vec2 rayEnd = center + _length * rayDir;
        auto hit = raycastNearestObject(game, center, rayEnd, [](auto obj){ return hasCollisionCategory(obj, SHELL_CATEGORY); });
        if (hit) {
            auto [obj, fraction] = *hit;
            obj->damage(1, DamageType::THERMAL);
            _length *= fraction;
        }
    }
}

Message LaserState::message() const {
    auto msg = BaseObjectState::message();
    msg["length"] = _length;
    return msg;
}
