#include "Tank.hpp"

#include <QDebug>
#include <random>

#include "../Game.hpp"
#include "common/Constants.hpp"
#include "actions/MainGun.hpp"
#include "actions/MachineGun.hpp"
#include "actions/RocketLauncher.hpp"
#include "actions/StatBoostModules.hpp"
#include "actions/DropModules.hpp"
#include "actions/StunModule.hpp"
#include "actions/MineDetector.hpp"

float TankState::maxHealth() const {
    float total = 50;
    for (auto& action : _actions) {
        if (action) {
            total += action->healthContribution();
        }
    }
    return total;
}

float TankState::maxSpeed() const {
    float total = 30;
    for (auto& action : _actions) {
        if (action) {
            total += action->speedContribution();
        }
    }
    return total;
}

float TankState::maxShield() const {
    float total = 0;
    for (auto& action : _actions) {
        if (action) {
            total += action->maxShieldContribution();
        }
    }
    return total;
}

void TankState::createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;

    BaseObjectState::createBodies(game, world, bodyDef);

    b2CircleShape box;
    // Half-size
    box.m_radius = 3.0f;

    // No friction to prevent rubbing on walls
    body()->CreateFixture(&box, 100.0)->SetFriction(0);
}

constexpr float sign(float a) { return a > 0 ? 1 : (a < 0 ? -1 : 0); }

float speedDiff(b2Vec2 velocity, b2Vec2 forward, float power, float maxTrackSpeed, float maxChange) {
    auto speed = b2Dot(velocity, forward);
    auto wantedSpeed = maxTrackSpeed * power;
    auto speedDiff = maxTrackSpeed * power - speed;
    if (sign(speed) != sign(wantedSpeed)) {
        // Decelerating
        speedDiff = std::min(std::abs(speedDiff), maxChange) * sign(speedDiff);
    }
    else {
        // Accelerating
        speedDiff = std::min(std::abs(speedDiff), maxChange * std::abs(power)) * sign(speedDiff);
    }
    return speedDiff;
}

void TankState::prePhysics(Game* game) {
    BaseObjectState::prePhysics(game);
    auto maxTrackSpeed = maxSpeed();

    auto forward = body()->GetWorldVector({1, 0});
    auto sideways = b2Vec2{forward.y, -forward.x};
    auto velocity = body()->GetLinearVelocity();

    // Rotational and sideways friction
    auto frontVelocity = body()->GetLinearVelocityFromLocalPoint({-3, 0});
    auto backVelocity = body()->GetLinearVelocityFromLocalPoint({3, 0});
    auto leftVelocity = body()->GetLinearVelocityFromLocalPoint({0, 3});
    auto rightVelocity = body()->GetLinearVelocityFromLocalPoint({0, -3});
    _leftTrackMovement += b2Dot(leftVelocity, forward) * game->timestep();
    _rightTrackMovement += b2Dot(rightVelocity, forward) * game->timestep();
    body()->ApplyLinearImpulse(-b2Dot(frontVelocity, sideways) * 300 * sideways, body()->GetWorldPoint({-3, 0}), true);
    body()->ApplyLinearImpulse(-b2Dot(backVelocity, sideways) * 300 * sideways, body()->GetWorldPoint({3, 0}), true);

    // Friction
    body()->ApplyLinearImpulseToCenter(-b2Dot(velocity, forward) * 100 * forward, true);

    if (stunned()) {
        return;
    }

    // Movement
    body()->ApplyLinearImpulse(speedDiff(leftVelocity, forward, _leftTrack, maxTrackSpeed, maxTrackSpeed) * 300 * forward, body()->GetWorldPoint({0, 3}), true);
    body()->ApplyLinearImpulse(speedDiff(rightVelocity, forward, _rightTrack, maxTrackSpeed, maxTrackSpeed) * 300 * forward, body()->GetWorldPoint({0, -3}), true);

    float angleDiff = std::remainder(_targetTurretAngle - _turretAngle, 2.0f * M_PI);
    _turretAngle += std::min(_slewRate, std::max(-_slewRate, angleDiff));
    for (auto& action : _actions) {
        if (action) {
            action->prePhysics(game, this);
        }
    }
}

void TankState::postPhysics(Game* game) {
    for (auto& action : _actions) {
        if (action) {
            action->postPhysics(game, this);
        }
    }
}

std::unique_ptr<TankModule> createModule(int type) {
    switch (type) {
        case 0: return std::make_unique<MainGun>();
        case 1: return std::make_unique<RocketLauncher>();
        case 2: return std::make_unique<MachineGun>();
        case 3: return std::make_unique<HealthModule>();
        case 4: return std::make_unique<SpeedModule>();
        case 5: return std::make_unique<ShieldModule>();
        case 6: return std::make_unique<Laser>();
        case 7: return std::make_unique<CrateModule>();
        case 8: return std::make_unique<BombModule>();
        case 9: return std::make_unique<HomingRocketLauncher>();
        case 10: return std::make_unique<StunModule>();
        case 11: return std::make_unique<InvisibiltyModule>();
        case 12: return std::make_unique<TimedBombModule>();
        case 13: return std::make_unique<MGTurretModule>();
        case 14: return std::make_unique<MineModule>();
        case 15: return std::make_unique<MineDetector>();
    }
    return {};
}

void TankState::handleMessage(const bt_messages::ToServerMessage_ControlState& msg) {
    _targetTurretAngle = std::remainder(msg.turret_angle(), 2.0f * M_PI);
    _leftTrack = msg.left_track();
    _rightTrack = msg.right_track();
    auto actionVecIter = msg.actions().begin();
    auto actionsIter = _actions.begin();
    for (; actionVecIter != msg.actions().end() && actionsIter != _actions.end(); ++actionVecIter, ++actionsIter) {
        if (*actionsIter) {
            (*actionsIter)->setActive(*actionVecIter);
        }
    }
}

void TankState::setModules(const std::vector<int>& modules) {
    for (auto module : modules) {
        _actions.emplace_back(createModule(module));
    }
    _shield = maxShield();
}

void TankState::damage(float amount, DamageType type) {
    switch (type) {
        case DamageType::IMPACT: case DamageType::THERMAL: {
            auto shieldDeduction = std::min(amount, _shield);
            _shield -= shieldDeduction;
            amount -= shieldDeduction;
            break;
        }
        case DamageType::PIERCING: {
            auto shieldDeduction = std::min(amount / 2, _shield);
            _shield -= shieldDeduction;
            amount -= shieldDeduction;
            break;
        }
    }
    BaseObjectState::damage(amount, type);
}

void TankState::stun(int amount) {
    float multiplier = 1.0;
    for (auto& action : _actions) {
        if (action) {
            multiplier *= action->stunResistanceMultiplier();
        }
    }
    BaseObjectState::stun(amount * multiplier);
}


void TankState::addShield(float amount) {
    _shield = std::max(0.0f, std::min(_shield + amount, maxShield()));
}

void TankState::fillMessage(bt_messages::ToClientMessage_ObjectUpdated& msg) const {
    BaseObjectState::fillMessage(msg);
    auto& tank_updates = *msg.mutable_tank_updates();
    tank_updates.set_shield(_shield / maxShield());
    msg.set_turret_angle(_turretAngle);
    for (auto& action : _actions) {
        auto& module_updates = *tank_updates.add_modules();
        if (action) {
            action->fillMessage(module_updates);
        }
        else {
            module_updates.set_type(-1);
        }
    }
    tank_updates.set_left_track_movement(_leftTrackMovement);
    tank_updates.set_right_track_movement(_rightTrackMovement);
}
