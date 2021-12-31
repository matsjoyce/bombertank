#ifndef OBJECTS_TANK_CONTROL_HPP
#define OBJECTS_TANK_CONTROL_HPP

#include <QObject>
#include <QQmlEngine>

#include "common/TcpMessageSocket.hpp"

class TankControlState : public QObject {
    Q_OBJECT
    Q_PROPERTY(float leftTrack READ leftTrack WRITE setLeftTrack NOTIFY leftTrackChanged)
    Q_PROPERTY(float rightTrack READ rightTrack WRITE setRightTrack NOTIFY rightTrackChanged)
    QML_ELEMENT

    float _leftTrack, _rightTrack;
    std::vector<bool> _actions;

   public:
    float leftTrack() const { return _leftTrack; }
    void setLeftTrack(float power);

    float rightTrack() const { return _rightTrack; }
    void setRightTrack(float power);

    Q_INVOKABLE bool action(int idx) { return _actions.size() < idx ? _actions[idx] : false; }
    Q_INVOKABLE void setAction(int idx, bool active);

    Message message() const;

   signals:
    void leftTrackChanged();
    void rightTrackChanged();
    void actionChanged(int idx);
};

#endif  // OBJECTS_TANK_CONTROL_HPP
