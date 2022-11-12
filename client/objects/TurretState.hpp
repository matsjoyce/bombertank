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
    void loadMessage(Message& msg) override;

private:
    Q_OBJECT_BINDABLE_PROPERTY(TurretState, float, _turretAngleProp)
};

#endif // OBJECTS_TURRET_STATE_HPP

