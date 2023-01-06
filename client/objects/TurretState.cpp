#include "TurretState.hpp"

void TurretState::loadMessage(const bt_messages::ToClientMessage_ObjectUpdated& msg) {
    BaseObjectState::loadMessage(msg);
    _turretAngleProp.setValue(msg.turret_angle());
}

void LaserState::loadMessage(const bt_messages::ToClientMessage_ObjectUpdated& msg) {
    BaseObjectState::loadMessage(msg);
    _lengthProp.setValue(msg.length());
}
