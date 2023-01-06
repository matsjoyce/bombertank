#ifndef OBJECTS_TANK_CONTROL_HPP
#define OBJECTS_TANK_CONTROL_HPP

#include <QObject>
#include <QProperty>
#include <QQmlEngine>

#include "common/TcpMessageSocket.hpp"

class TankControlState : public QObject {
    Q_OBJECT
    Q_PROPERTY(float turretAngle READ turretAngle WRITE setTurretAngle BINDABLE bindableTurretAngle)
    Q_PROPERTY(float leftTrack READ leftTrack WRITE setLeftTrack BINDABLE bindableLeftTrack)
    Q_PROPERTY(float rightTrack READ rightTrack WRITE setRightTrack BINDABLE bindableRightTrack)
    QML_ELEMENT

    std::vector<bool> _actions;

   public:
    float leftTrack() const { return _leftTrackProp.value(); }
    void setLeftTrack(float value) { _leftTrackProp.setValue(value); emit controlsChanged(); }
    QBindable<float> bindableLeftTrack() { return &_leftTrackProp; }

    float rightTrack() const { return _rightTrackProp.value(); }
    void setRightTrack(float value) { _rightTrackProp.setValue(value); emit controlsChanged(); }
    QBindable<float> bindableRightTrack() { return &_rightTrackProp; }

    float turretAngle() const { return _turretAngleProp.value(); }
    void setTurretAngle(float value) { _turretAngleProp.setValue(value); emit controlsChanged(); }
    QBindable<float> bindableTurretAngle() { return &_turretAngleProp; }

    Q_INVOKABLE bool action(int idx) { return _actions.size() < idx ? _actions[idx] : false; }
    Q_INVOKABLE void setAction(int idx, bool active);

    void fillMessage(bt_messages::ToServerMessage_ControlState& msg) const;

   signals:
    void actionChanged(int idx);
    void controlsChanged();

   private:
    Q_OBJECT_BINDABLE_PROPERTY(TankControlState, float, _leftTrackProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankControlState, float, _rightTrackProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankControlState, float, _turretAngleProp)
};

#endif  // OBJECTS_TANK_CONTROL_HPP
