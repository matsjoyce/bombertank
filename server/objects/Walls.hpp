#ifndef OBJECTS_WALLS_HPP
#define OBJECTS_WALLS_HPP

#include "Base.hpp"

class AbstractWallState : public BaseObjectState {
   public:
    using BaseObjectState::BaseObjectState;
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
};

class WallState : public AbstractWallState {
   public:
    WallState();
};

class IndestructableWallState : public AbstractWallState {
   public:
    IndestructableWallState();
};

#endif  // OBJECTS_WALLS_HPP