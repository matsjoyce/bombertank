#include "Turret.hpp"

#include <QDebug>
#include <random>

#include "../Game.hpp"
#include "common/Constants.hpp"
#include "Projectiles.hpp"
#include "Utils.hpp"
#include "Queries.hpp"

float TurretState::maxHealth() const {
    return 50;
}

void TurretState::createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_staticBody;

    BaseObjectState::createBodies(game, world, bodyDef);

    b2PolygonShape box;
    // Half-size
    box.SetAsBox(3.0f, 3.0f);

    body()->CreateFixture(&box, 100.0);
    body()->SetFixedRotation(true);
    _targetTurretAngle = body()->GetAngle();
}

void TurretState::prePhysics(Game* game) {
    BaseObjectState::prePhysics(game);
    if (stunned()) {
        return;
    }
    auto center = body()->GetPosition();
    auto targetPosition = center;
    bool hasTarget = false;
    auto prioritisedTargets = getPrioritsedTargets(queryObjectsInCircle(game, center, _targettingRange), side(), center);
    if (prioritisedTargets.size()) {
        targetPosition = prioritisedTargets.front()->body()->GetPosition();
        auto targetVector = (targetPosition - center);
        _targetTurretAngle = std::atan2(targetVector.y, targetVector.x);
        hasTarget = true;
    }

    float angleDiff = std::remainder(_targetTurretAngle - _turretAngle, 2.0f * M_PI);
    _turretAngle += std::min(_slewRate, std::max(-_slewRate, angleDiff));

    if (hasTarget && std::abs(angleDiff) < 0.1) {
        auto raycastResult = raycastNearestObject(game, center, targetPosition, [](auto obj) { return true; });
        if (raycastResult && std::get<0>(*raycastResult) == prioritisedTargets.front()) {
            fire(_turretAngle, game);
        }
    }
}

Message TurretState::message() const {
    auto msg = BaseObjectState::message();
    msg["turretAngle"] = _turretAngle;
    return msg;
}

void LaserTurretState::prePhysics(Game* game) {
    if (!_laser) {
        auto obj = game->addObject(constants::ObjectType::LASER, body()->GetPosition(), 0, {0, 0});
        if (obj) {
            _laser = dynamic_cast<LaserState*>(obj->second);
        }
    }
    if (_energy < 20) {
        ++_energy;
    }
    else {
        _armed = true;
    }
    _laser->setMaxLength(0);
    TurretState::prePhysics(game);
}

void LaserTurretState::postPhysics(Game* game) {
    if (dead() && _laser) {
        _laser->die();
    }
}

void LaserTurretState::fire(float angle, Game* game) {
    auto forward = b2Vec2{std::cos(angle), std::sin(angle)};
    _laser->body()->SetTransform(body()->GetPosition() + 3 * forward, angle);
    if (_energy > 2 && _armed) {
        _laser->setMaxLength(100);
        _energy -= 2;
        return;
    }
    else {
        _armed = false;
    }
}

void MachineGunTurretState::prePhysics(Game* game) {
    if (_reload) {
        --_reload;
    }
    TurretState::prePhysics(game);
}

void MachineGunTurretState::fire(float angle, Game* game) {
    if (!_reload) {
        auto forward = b2Vec2{std::cos(angle), std::sin(angle)};
        auto sideways = b2Vec2{forward.y, -forward.x};

        const auto speed = 100;
        const auto maxSidewaysVelocity = speed / 20;
        std::uniform_real_distribution<float> distribution(-maxSidewaysVelocity, maxSidewaysVelocity);
        auto velocity = speed * forward + distribution(game->randomGenerator()) * sideways;
        game->addObject(constants::ObjectType::MG_SHELL, body()->GetPosition() + 4.5 * forward,
                                        std::atan2(velocity.y, velocity.x), velocity);
        _reload = 2;
        return;
    }
}
