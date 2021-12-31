#include "Base.hpp"

void BaseObjectState::loadMessage(Message& msg) {
    _type = static_cast<constants::ObjectType>(msg["type"].as_uint64_t());
    _xProp.setValue(msg["x"].as_double());
    _yProp.setValue(msg["y"].as_double());
    _rotationProp.setValue(msg["rotation"].as_double());
    _healthProp.setValue(msg["health"].as_double());
    _sideProp.setValue(msg["side"].as_uint64_t());
    _speedProp.setValue(std::hypot(msg["vx"].as_double(), msg["vy"].as_double()));
}

void BaseObjectState::setFromEditor(constants::ObjectType type, float x, float y) {
    _type = type;
    _xProp.setValue(x);
    _yProp.setValue(y);
}
