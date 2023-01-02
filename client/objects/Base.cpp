#include "Base.hpp"
#include "common/MsgpackUtils.hpp"

void BaseObjectState::loadMessage(Message& msg) {
    _typeProp.setValue(msg["type"].as_uint64_t());
    _xProp.setValue(extractDouble(msg["x"]));
    _yProp.setValue(extractDouble(msg["y"]));
    _rotationProp.setValue(extractDouble(msg["rotation"]));
    _healthProp.setValue(extractDouble(msg["health"]));
    _sideProp.setValue(msg["side"].as_uint64_t());
    _speedProp.setValue(std::hypot(extractDouble(msg["vx"]), extractDouble(msg["vy"])));
    _statusProp.setValue(msg["status"].as_uint64_t());
}

void BaseObjectState::setFromEditor(constants::ObjectType type, float x, float y) {
    _typeProp.setValue(static_cast<int>(type));
    _xProp.setValue(x);
    _yProp.setValue(y);
}
