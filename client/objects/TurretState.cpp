#include "TurretState.hpp"
#include "common/MsgpackUtils.hpp"

void TurretState::loadMessage(Message& msg) {
    BaseObjectState::loadMessage(msg);
    _turretAngleProp.setValue(extractDouble(msg["turretAngle"]));
}

void LaserState::loadMessage(Message& msg) {
    BaseObjectState::loadMessage(msg);
    _lengthProp.setValue(extractDouble(msg["length"]));
}
