#ifndef OBJECTS_TANK_STATE_HPP
#define OBJECTS_TANK_STATE_HPP

#include "Base.hpp"

class TankState : public BaseObjectState {
    Q_OBJECT
    Q_PROPERTY(float shield READ shield BINDABLE bindableShield)
    QML_ELEMENT

public:
    float shield() { return _shieldProp.value(); }
    QBindable<float> bindableShield() { return &_shieldProp; }
    void loadMessage(Message& msg) override;

private:
    Q_OBJECT_BINDABLE_PROPERTY(TankState, float, _shieldProp)
};

#endif // OBJECTS_TANK_STATE_HPP
