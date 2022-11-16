#include "Tank.hpp"

#include <QDebug>
#include <random>

#include "../Game.hpp"
#include "common/Constants.hpp"
#include "actions/MainGun.hpp"
#include "actions/MachineGun.hpp"
#include "actions/RocketLauncher.hpp"
#include "actions/StatBoostModules.hpp"

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

    body()->CreateFixture(&box, 100.0);
    body()->SetFixedRotation(true);
}

void TankState::prePhysics(Game* game) {
    if (_power) {
        body()->SetTransform(body()->GetPosition(), _angle);
        body()->SetLinearVelocity(maxSpeed() * _power * body()->GetWorldVector({1, 0}));
    }
    else {
        body()->SetLinearVelocity({0, 0});
    }
    auto angleDiff = std::fmod(_targetTurretAngle - _turretAngle, 2.0f * M_PIf);
    if (angleDiff > M_PI) {
        angleDiff -= 2 * M_PI;
    }
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
        case 3: return std::make_unique<SpeedModule>();
        case 4: return std::make_unique<HealthModule>();
        case 5: return std::make_unique<ShieldModule>();
    }
    return {};
}

void TankState::handleMessage(const Message& msg) {
    if (msg.at("cmd").as_string() == "control_state") {
        _angle = msg.at("angle").as_double();
        _targetTurretAngle = std::fmod(msg.at("turretAngle").as_double(), 2.0f * M_PIf);
        _power = msg.at("power").as_double();

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
