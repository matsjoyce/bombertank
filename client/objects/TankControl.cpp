#include "TankControl.hpp"

Message TankControlState::message() const {
    return {
        {"left_track", _leftTrack},
        {"right_track", _rightTrack},
        {"actions", std::vector<msgpack::type::variant>(_actions.begin(), _actions.end())}};
}

void TankControlState::setLeftTrack(float power) {
    if (_leftTrack != power) {
        _leftTrack = power;
        emit leftTrackChanged();
    }
}

void TankControlState::setRightTrack(float power) {
    if (_rightTrack != power) {
        _rightTrack = power;
        emit rightTrackChanged();
    }
}

void TankControlState::setAction(int idx, bool active) {
    if (_actions.size() <= idx) {
        _actions.resize(idx + 1);
    }
    if (_actions[idx] != active) {
        _actions[idx] = active;
        emit actionChanged(idx);
    }
}