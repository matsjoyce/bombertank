#include "Tank.hpp"

#include <QDebug>

#include "../Game.hpp"
#include "common/Constants.hpp"

TankState::TankState() { _actions.resize(5); }

float TankState::maxHealth() const { return 150; }

void TankState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;
    // Prevent the tank spinning too fast
    bodyDef.angularDamping = 10;

    BaseObjectState::createBodies(world, bodyDef);

    b2PolygonShape box;
    // Half-size
    box.SetAsBox(3.0f, 3.0f);

    body()->CreateFixture(&box, 100.0);

    b2BodyDef trackDef;
    trackDef.type = b2_kinematicBody;
    trackDef.position = bodyDef.position;

    _leftTrackBody = world.CreateBody(&trackDef);
    b2FrictionJointDef frictionDef;
    frictionDef.bodyA = body();
    frictionDef.localAnchorA = {0, 3};
    frictionDef.bodyB = _leftTrackBody;
    frictionDef.maxForce = 10 * body()->GetMass() * 2;
    _leftTrackJoint = world.CreateJoint(&frictionDef);

    _rightTrackBody = world.CreateBody(&trackDef);
    frictionDef.localAnchorA = {0, -3};
    frictionDef.bodyB = _rightTrackBody;
    _rightTrackJoint = world.CreateJoint(&frictionDef);
}

constexpr float sign(float a) { return a > 0 ? 1 : (a < 0 ? -1 : 0); }

float speedDiff(b2Body* body, b2Vec2 offset, b2Vec2 forward, float power, float maxTrackSpeed, float maxChange) {
    auto speed = b2Dot(body->GetLinearVelocityFromLocalPoint(offset), forward);
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
    return speedDiff + speed;
}

void TankState::prePhysics(Game* game) {
    auto maxTrackSpeed = 20.0f;
    auto maxAccel = 5.0f;

    auto forward = body()->GetWorldVector({1, 0});

    _leftTrackBody->SetTransform(body()->GetWorldPoint({0, 3}), 0);
    _leftTrackBody->SetLinearVelocity(speedDiff(body(), {0, 3}, forward, _leftTrack, maxTrackSpeed, maxAccel) *
                                      forward);

    _rightTrackBody->SetTransform(body()->GetWorldPoint({0, -3}), 0);
    _rightTrackBody->SetLinearVelocity(speedDiff(body(), {0, -3}, forward, _rightTrack, maxTrackSpeed, maxAccel) *
                                       forward);

    if (_actions[0]) {
        // Shoot
        qDebug() << "Create shell";
        auto shell = game->addObject(constants::ObjectType::SHELL, body()->GetPosition() + 3.5 * forward,
                                     body()->GetAngle(), 40 * forward);
        if (shell) {
            body()->ApplyLinearImpulseToCenter(-40 * shell->body()->GetMass() * forward, true);
        }
    }
}

void TankState::handleMessage(const Message& msg) {
    if (msg.at("cmd").as_string() == "control_state") {
        _leftTrack = msg.at("left_track").as_double();
        _rightTrack = msg.at("right_track").as_double();
        _actions.clear();
        auto actionsVec = msg.at("actions").as_vector();
        std::transform(actionsVec.begin(), actionsVec.end(), std::back_inserter(_actions),
                       [](auto& o) { return o.as_bool(); });
        _actions.resize(5);
    }
}

void TankState::damage(float amount, DamageType type) { BaseObjectState::damage(amount, type); }