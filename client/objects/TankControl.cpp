#include "TankControl.hpp"

void TankControlState::fillMessage(bt_messages::ToServerMessage_ControlState& msg) const {
    msg.set_turret_angle(_turretAngleProp.value());
    msg.set_left_track(_leftTrackProp.value());
    msg.set_right_track(_rightTrackProp.value());
    for (auto action : _actions) {
        msg.add_actions(action);
    }
}

void TankControlState::setAction(int idx, bool active) {
    if (_actions.size() <= idx) {
        _actions.resize(idx + 1);
    }
    if (_actions[idx] != active) {
        _actions[idx] = active;
        emit actionChanged(idx);
        emit controlsChanged();
    }
}
