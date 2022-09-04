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
    frictionDef.maxForce = 10 * body()->GetMass() * 2;
    world.CreateJoint(&frictionDef);
}

void BombState::destroy(Game* game) {
    game->addObject(constants::ObjectType::EXPLOSION, body()->GetPosition(), 0, {0, 0});
    CrateState::destroy(game);
}

float BombState::maxHealth() const {
    return 10;
}

