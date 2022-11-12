#include "TurretState.hpp"

void TurretState::loadMessage(Message& msg) {
    BaseObjectState::loadMessage(msg);
    _turretAngleProp.setValue(msg["turretAngle"].as_double());
}
