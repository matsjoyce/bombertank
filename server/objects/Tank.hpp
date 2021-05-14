#ifndef OBJECTS_TANK_HPP
#define OBJECTS_TANK_HPP

#include "Base.hpp"

class TankState : public BaseObjectState {
    float _leftTrack, _rightTrack;
    std::vector<bool> _actions;

    b2Body *_leftTrackBody = nullptr, *_rightTrackBody = nullptr;
    b2Joint *_leftTrackJoint = nullptr, *_rightTrackJoint = nullptr;

   public:
    TankState(constants::ObjectType type_);
    void createBodies(b2World& world, b2BodyDef& bodyDef) override;
    void prePhysics(Game* game) override;
    void handleMessage(const Message& msg) override;
    void damage(float amount, DamageType type) override;
};

#endif  // OBJECTS_TANK_HPP