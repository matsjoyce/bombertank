#include "Base.hpp"

void BaseObjectState::loadMessage(const bt_messages::ToClientMessage_ObjectUpdated& msg) {
    _typeProp.setValue(msg.type());
    _xProp.setValue(msg.x());
    _yProp.setValue(msg.y());
    _rotationProp.setValue(msg.rotation());
    _healthProp.setValue(msg.health());
    _sideProp.setValue(msg.side());
    _speedProp.setValue(std::hypot(msg.vx(), msg.vy()));
    _statusProp.setValue(msg.status());
    _destroyedProp.setValue(msg.destroyed());
}

void BaseObjectState::setFromEditor(constants::ObjectType type, float x, float y, float rotation) {
    _typeProp.setValue(static_cast<int>(type));
    _xProp.setValue(x);
    _yProp.setValue(y);
    _rotationProp.setValue(rotation);
}
