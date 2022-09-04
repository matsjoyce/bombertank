#include "Tank.hpp"

#include <QDebug>
#include <random>

#include "../Game.hpp"
#include "common/Constants.hpp"
#include "actions/MainGun.hpp"
#include "actions/MachineGun.hpp"
#include "actions/RocketLauncher.hpp"

TankState::TankState() {
}

float TankState::maxHealth() const { return 150; }

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
        body()->SetLinearVelocity(50.0f * _power * body()->GetWorldVector({1, 0}));
    }
    else {
        body()->SetLinearVelocity({0, 0});
    }
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
    }
    return {};
}

void TankState::handleMessage(const Message& msg) {
    if (msg.at("cmd").as_string() == "control_state") {
        _angle = msg.at("angle").as_double();
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
    }
}

void TankState::damage(float amount, DamageType type) {
    BaseObjectState::damage(amount, type);
}
