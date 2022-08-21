#ifndef OBJECTS_TANK_CONTROL_HPP
#define OBJECTS_TANK_CONTROL_HPP

#include <QObject>
#include <QProperty>
#include <QQmlEngine>

#include "common/TcpMessageSocket.hpp"

class TankControlState : public QObject {
    Q_OBJECT
    Q_PROPERTY(float angle READ angle WRITE setAngle BINDABLE bindableAngle)
    Q_PROPERTY(float power READ power WRITE setPower BINDABLE bindablePower)
    QML_ELEMENT

    float _leftTrack, _rightTrack;
    std::vector<bool> _actions;

   public:
    float p() const { return _leftTrack; }
    void setLeftTrack(float power);

    float rightTrack() const { return _rightTrack; }
    void setRightTrack(float power);

    Q_INVOKABLE bool action(int idx) { return _actions.size() < idx ? _actions[idx] : false; }
    Q_INVOKABLE void setAction(int idx, bool active);

    Message message() const;

    float angle() const { return _angleProp.value(); }
    void setAngle(float value) { _angleProp.setValue(value); }
    QBindable<float> bindableAngle() { return &_angleProp; emit controlsChanged(); }
    float power() const { return _powerProp.value(); }
    void setPower(float value) { _powerProp.setValue(value); emit controlsChanged(); }
    QBindable<float> bindablePower() { return &_powerProp; }

   signals:
    void actionChanged(int idx);
    void controlsChanged();

   private:
    Q_OBJECT_BINDABLE_PROPERTY(TankControlState, float, _angleProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankControlState, float, _powerProp)

};

#endif  // OBJECTS_TANK_CONTROL_HPP
