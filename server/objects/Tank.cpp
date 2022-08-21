#include "Tank.hpp"

#include <QDebug>
#include <random>

#include "../Game.hpp"
#include "common/Constants.hpp"

TankState::TankState() { _actions.resize(5); }

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
    auto forward = body()->GetWorldVector({1, 0});
    if (_power) {
        body()->SetTransform(body()->GetPosition(), _angle);
        body()->SetLinearVelocity(50.0f * _power * body()->GetWorldVector({1, 0}));
    }
    else {
        body()->SetLinearVelocity({0,0});
    }

    if (_reload) {
        --_reload;
    }

    if (_actions[0] && !_reload) {
        // Shoot
        qDebug() << "Create shell";
        auto sideways = body()->GetWorldVector({0, 1});

        const auto speed = 200;
        const auto maxSidewaysVelocity = speed / 20;
        std::uniform_real_distribution<float> distribution(-maxSidewaysVelocity, maxSidewaysVelocity);
        auto velocity = speed * forward + distribution(game->randomGenerator()) * sideways;
        auto shell = game->addObject(constants::ObjectType::SHELL, body()->GetPosition() + 3.5 * forward,
                                     std::atan2(velocity.y, velocity.x), velocity);
        if (shell) {
            body()->ApplyLinearImpulseToCenter(-velocity.Length() * shell->body()->GetMass() * forward, true);
        }
        _reload = 2;
    }
}

void TankState::handleMessage(const Message& msg) {
    if (msg.at("cmd").as_string() == "control_state") {
        _angle = msg.at("angle").as_double();
        _power = msg.at("power").as_double();
        _actions.clear();
        auto actionsVec = msg.at("actions").as_vector();
        std::transform(actionsVec.begin(), actionsVec.end(), std::back_inserter(_actions),
                       [](auto& o) { return o.as_bool(); });
        _actions.resize(5);
    }
}

void TankState::damage(float amount, DamageType type) {
    if (type == DamageType::IMPACT && amount < 10) {
        return;
    }
//     BaseObjectState::damage(amount, type);
}
