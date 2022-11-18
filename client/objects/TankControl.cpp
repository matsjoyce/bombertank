#include "TankControl.hpp"

Message TankControlState::message() const {
    return {
        {"turretAngle", _turretAngleProp.value()},
        {"left_track", _leftTrackProp.value()},
        {"right_track", _rightTrackProp.value()},
        {"actions", std::vector<msgpack::type::variant>(_actions.begin(), _actions.end())}};
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
