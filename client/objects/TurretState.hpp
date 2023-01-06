#ifndef OBJECTS_TURRET_STATE_HPP
#define OBJECTS_TURRET_STATE_HPP

#include "Base.hpp"

class TurretState : public BaseObjectState {
    Q_OBJECT
    Q_PROPERTY(float turretAngle READ turretAngle BINDABLE bindableTurretAngle)
    QML_ELEMENT

public:
    float turretAngle() { return _turretAngleProp.value(); }
    QBindable<float> bindableTurretAngle() { return &_turretAngleProp; }
    void loadMessage(const bt_messages::ToClientMessage_ObjectUpdated& msg) override;

private:
    Q_OBJECT_BINDABLE_PROPERTY(TurretState, float, _turretAngleProp)
};

class LaserState : public BaseObjectState {
    Q_OBJECT
    Q_PROPERTY(float length READ length BINDABLE bindableLength)
    QML_ELEMENT

public:
    float length() { return _lengthProp.value(); }
    QBindable<float> bindableLength() { return &_lengthProp; }
    void loadMessage(const bt_messages::ToClientMessage_ObjectUpdated& msg) override;

signals:
    void lengthChanged(float value);

private:
    Q_OBJECT_BINDABLE_PROPERTY(LaserState, float, _lengthProp, &LaserState::lengthChanged)
};

#endif // OBJECTS_TURRET_STATE_HPP

