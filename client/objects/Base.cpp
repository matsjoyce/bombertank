#include "Base.hpp"

void BaseObjectState::loadMessage(Message& msg) {
    _type = static_cast<ObjectType>(msg["type"].as_uint64_t());
    _x = msg["x"].as_double();
    _y = msg["y"].as_double();
    _rotation = msg["rotation"].as_double();
    auto health = msg["health"].as_double();
    if (health != _health) {
        _health = health;
        emit healthChanged(health);
    }
}