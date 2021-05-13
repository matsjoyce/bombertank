#include "Walls.hpp"

#include <QDebug>

void AbstractWallState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_staticBody;

    BaseObjectState::createBodies(world, bodyDef);

    b2PolygonShape box;
    // Half-size
    box.SetAsBox(3.0f, 3.0f);

    body->CreateFixture(&box, 1.0);
}

WallState::WallState() : AbstractWallState(3, 100) {}

IndestructableWallState::IndestructableWallState() : AbstractWallState(4, 1) {}
