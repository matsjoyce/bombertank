#include "Tank.hpp"

#include <QDebug>
#include <random>

#include "../Game.hpp"
#include "common/Constants.hpp"
#include "actions/MainGun.hpp"
#include "actions/MachineGun.hpp"
#include "actions/RocketLauncher.hpp"

TankState::TankState() {
    _actions.emplace_back(std::make_unique<MainGun>());
    _actions.emplace_back(std::make_unique<MachineGun>());
    _actions.emplace_back(std::make_unique<RocketLauncher>());
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
        action->prePhysics(game, this);
    }
}

void TankState::postPhysics(Game* game) {
    for (auto& action : _actions) {
        action->postPhysics(game, this);
    }
}


void TankState::handleMessage(const Message& msg) {
    if (msg.at("cmd").as_string() == "control_state") {
        _angle = msg.at("angle").as_double();
        _power = msg.at("power").as_double();

        auto actionsVec = msg.at("actions").as_vector();
        auto actionVecIter = actionsVec.begin();
        auto actionsIter = _actions.begin();
        for (; actionVecIter != actionsVec.end() && actionsIter != _actions.end(); ++actionVecIter, ++actionsIter) {
            (*actionsIter)->setActive(actionVecIter->as_bool());
        }

    }
}

void TankState::damage(float amount, DamageType type) {
    BaseObjectState::damage(amount, type);
}
