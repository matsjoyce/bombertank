#include "Walls.hpp"

#include <QDebug>

void AbstractWallState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_staticBody;

    BaseObjectState::createBodies(world, bodyDef);

    b2PolygonShape box;
    // Half-size
    box.SetAsBox(4.5f, 4.5f);

    body()->CreateFixture(&box, 1.0);
}

float WallState::maxHealth() const { return 100; }

float IndestructableWallState::maxHealth() const { return 1; }
