#include "Crate.hpp"

#include <QDebug>

CrateState::CrateState() : BaseObjectState(2, 50) {}

void CrateState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;
    // Prevent the tank spinning too fast
    bodyDef.angularDamping = 30;

    BaseObjectState::createBodies(world, bodyDef);

    b2PolygonShape box;
    // Half-size
    box.SetAsBox(3.0f, 3.0f);

    body->CreateFixture(&box, 1.0);

    b2BodyDef groundDef;
    groundDef.type = b2_kinematicBody;
    groundDef.position = bodyDef.position;

    _groundBody = world.CreateBody(&groundDef);
    b2FrictionJointDef frictionDef;
    frictionDef.bodyA = body;
    frictionDef.bodyB = _groundBody;
    frictionDef.maxForce = 500;
    world.CreateJoint(&frictionDef);
}