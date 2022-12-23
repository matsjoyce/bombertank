#include "Crate.hpp"
#include "../Game.hpp"

#include <QDebug>

float CrateState::maxHealth() const { return 50; }

void CrateState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;
    // Prevent the crate spinning endlessly
    bodyDef.angularDamping = 30;

    BaseObjectState::createBodies(world, bodyDef);

    b2PolygonShape box;
    // Half-size
    box.SetAsBox(3.0f, 3.0f);

    body()->CreateFixture(&box, 10.0);

    b2BodyDef groundDef;
    groundDef.type = b2_kinematicBody;
    groundDef.position = bodyDef.position;

    _groundBody = world.CreateBody(&groundDef);
    b2FrictionJointDef frictionDef;
    frictionDef.bodyA = body();
    frictionDef.bodyB = _groundBody;
    frictionDef.maxForce = 100 * body()->GetMass();
    world.CreateJoint(&frictionDef);
}

void BombState::destroy(Game* game) {
    game->addObject(constants::ObjectType::EXPLOSION, body()->GetPosition(), 0, {0, 0});
    CrateState::destroy(game);
}

float BombState::maxHealth() const {
    return 10;
}

void TimedBombState::prePhysics(Game* game) {
    BombState::prePhysics(game);
    if (stunned()) {
        return;
    }
    if (_timer) {
        --_timer;
    }
    else {
        die();
    }
}

void MineState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;

    BaseObjectState::createBodies(world, bodyDef);

    b2CircleShape circ;
    circ.m_radius = 1;
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circ;
    fixtureDef.density = 100.0;
    fixtureDef.filter.maskBits = 0xFF & ~SHELL_CATEGORY & ~ROCKET_CATEGORY;

    body()->CreateFixture(&fixtureDef);
}

void MineState::prePhysics(Game* game) {
    BombState::prePhysics(game);
    invisiblize(1);
    if (stunned()) {
        return;
    }
    if (_timer) {
        --_timer;
    }
}

void MineState::collision(BaseObjectState* other, float impulse) {
    if (!_timer) {
        die();
    }
}
