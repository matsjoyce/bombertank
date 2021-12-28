#ifndef OBJECTS_BASE_HPP
#define OBJECTS_BASE_HPP

#include "common/Constants.hpp"
#include "common/TcpMessageSocket.hpp"

class BaseObjectState : public QObject {
    Q_OBJECT

    Q_PROPERTY(float health READ health NOTIFY healthChanged)
    Q_PROPERTY(int side READ side NOTIFY sideChanged)
    Q_PROPERTY(float speed READ speed NOTIFY speedChanged)

    constants::ObjectType _type;
    float _x = 0, _y = 0, _rotation = 0, _health = 0, _vx = 0, _vy = 0;
    int _side = 0;

   public:
    using QObject::QObject;
    void loadMessage(Message& msg);

    // Used by the MapView, so does not need to be QML accessable
    constants::ObjectType type() { return _type; }
    float x() { return _x; }
    float y() { return _y; }
    float rotation() { return _rotation; }

    // QML accessable
    float health() const { return _health; }
    int side() const { return _side; }
    float speed() const { return std::hypot(_vx, _vy); }

   signals:
    void healthChanged(float health);
    void sideChanged(int side);
    void speedChanged(float health);
};

#endif  // OBJECTS_BASE_HPP
