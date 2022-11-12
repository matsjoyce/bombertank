#include "TurretState.hpp"

void TurretState::loadMessage(Message& msg) {
    BaseObjectState::loadMessage(msg);
    _turretAngleProp.setValue(msg["turretAngle"].as_double());
}

void LaserState::loadMessage(Message& msg) {
    BaseObjectState::loadMessage(msg);
    _lengthProp.setValue(msg["length"].as_double());
}
