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

TankState::TankState() {
}

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

void TankState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;

    BaseObjectState::createBodies(world, bodyDef);

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
    auto maxTrackSpeed = maxSpeed();

    auto forward = body()->GetWorldVector({1, 0});
    auto sideways = b2Vec2{forward.y, -forward.x};
    auto velocity = body()->GetLinearVelocity();
    // Instant cancellation is body()->GetMass() I=v/m
    // Friction
    body()->ApplyLinearImpulseToCenter(-b2Dot(velocity, forward) * 100 * forward, true);

    // Rotational and sideways friction
    auto frontVelocity = body()->GetLinearVelocityFromLocalPoint({-3, 0});
    auto backVelocity = body()->GetLinearVelocityFromLocalPoint({3, 0});
    auto leftVelocity = body()->GetLinearVelocityFromLocalPoint({0, 3});
    auto rightVelocity = body()->GetLinearVelocityFromLocalPoint({0, -3});
    body()->ApplyLinearImpulse(-b2Dot(frontVelocity, sideways) * 300 * sideways, body()->GetWorldPoint({-3, 0}), true);
    body()->ApplyLinearImpulse(-b2Dot(backVelocity, sideways) * 300 * sideways, body()->GetWorldPoint({3, 0}), true);

    // Movement
    body()->ApplyLinearImpulse(speedDiff(leftVelocity, forward, _leftTrack, maxTrackSpeed, maxTrackSpeed) * 300 * forward, body()->GetWorldPoint({0, 3}), true);
    body()->ApplyLinearImpulse(speedDiff(rightVelocity, forward, _rightTrack, maxTrackSpeed, maxTrackSpeed) * 300 * forward, body()->GetWorldPoint({0, -3}), true);

    auto angleDiff = std::remainder(_targetTurretAngle - _turretAngle, 2.0f * M_PIf);
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
    }
    return {};
}

void TankState::handleMessage(const Message& msg) {
    if (msg.at("cmd").as_string() == "control_state") {
        _targetTurretAngle = std::remainder(msg.at("turretAngle").as_double(), 2.0f * M_PIf);
        _leftTrack = msg.at("left_track").as_double();
        _rightTrack = msg.at("right_track").as_double();
        auto actionsVec = msg.at("actions").as_vector();
        auto actionVecIter = actionsVec.begin();
        auto actionsIter = _actions.begin();
        for (; actionVecIter != actionsVec.end() && actionsIter != _actions.end(); ++actionVecIter, ++actionsIter) {
            if (*actionsIter) {
                (*actionsIter)->setActive(actionVecIter->as_bool());
            }
        }
    }
    else if (msg.at("cmd").as_string() == "join_game" && msg.at("tank_modules").is_vector()) {
        auto modulesVec = msg.at("tank_modules").as_vector();
        for (auto module : modulesVec) {
            _actions.emplace_back(createModule(module.is_uint64_t() ? module.as_uint64_t() : module.as_int64_t()));
        }
        _shield = maxShield();
    }
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

void TankState::addShield(float amount) {
    _shield = std::max(0.0f, _shield + amount);
}

Message TankState::message() const {
    auto msg = BaseObjectState::message();
    msg["shield"] = _shield / maxShield();
    msg["turretAngle"] = _turretAngle;
    std::vector<msgpack::type::variant> moduleMsgs;
    for (auto& action : _actions) {
        if (action) {
            auto msg = action->message();
            std::map<msgpack::type::variant, msgpack::type::variant> convertedMsg;
            for (auto& item : msg) {
                convertedMsg.emplace(item);
            }
            moduleMsgs.emplace_back(convertedMsg);
        }
        else {
            moduleMsgs.emplace_back(std::map<msgpack::type::variant, msgpack::type::variant>{{"type", -1}});
        }
    }
    msg["modules"] = moduleMsgs;
    return msg;
}
