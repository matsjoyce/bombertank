#include "Base.hpp"

void BaseObjectState::loadMessage(Message& msg) {
    _type = static_cast<constants::ObjectType>(msg["type"].as_uint64_t());
    _x = msg["x"].as_double();
    _y = msg["y"].as_double();
    _rotation = msg["rotation"].as_double();

    auto health = msg["health"].as_double();
    if (health != _health) {
        _health = health;
        emit healthChanged(health);
    }

    auto side = msg["side"].as_uint64_t();
    if (side != _side) {
        _side = side;
        emit sideChanged(side);
    }
}