#ifndef OBJECTS_BASE_HPP
#define OBJECTS_BASE_HPP

#include "common/Constants.hpp"
#include "common/TcpMessageSocket.hpp"

class BaseObjectState : public QObject {
    Q_OBJECT

    Q_PROPERTY(float health READ health NOTIFY healthChanged)

    constants::ObjectType _type;
    float _x = 0, _y = 0, _rotation = 0, _health = 0;

   public:
    using QObject::QObject;
    void loadMessage(Message& msg);

    constants::ObjectType type() { return _type; }
    float x() { return _x; }
    float y() { return _y; }
    float rotation() { return _rotation; }
    float health() { return _health; }

   signals:
    void healthChanged(float health);
};

#endif  // OBJECTS_BASE_HPP