#include "TankState.hpp"

void TankState::loadMessage(Message& msg) {
    BaseObjectState::loadMessage(msg);
    _shieldProp.setValue(msg["shield"].as_double());
}
