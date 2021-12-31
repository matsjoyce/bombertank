#ifndef OBJECTS_BASE_HPP
#define OBJECTS_BASE_HPP

#include <QObject>
#include <QQmlEngine>

#include "common/Constants.hpp"
#include "common/TcpMessageSocket.hpp"

class BaseObjectState : public QObject {
    Q_OBJECT
    Q_PROPERTY(float x READ x BINDABLE bindableX)
    Q_PROPERTY(float y READ y BINDABLE bindableY)
    Q_PROPERTY(float rotation READ rotation BINDABLE bindableRotation)
    Q_PROPERTY(float health READ health BINDABLE bindableHealth)
    Q_PROPERTY(int side READ side BINDABLE bindableSide)
    Q_PROPERTY(float speed READ speed BINDABLE bindableSpeed)
    QML_ELEMENT

    constants::ObjectType _type;

   public:
    using QObject::QObject;
    void loadMessage(Message& msg);
    void setFromEditor(constants::ObjectType type, float x, float y);

    constants::ObjectType type() { return _type; }
    float x() { return _xProp.value(); }
    QBindable<float> bindableX() { return &_xProp; }
    float y() { return _yProp.value(); }
    QBindable<float> bindableY() { return &_yProp; }
    float rotation() { return _rotationProp.value(); }
    QBindable<float> bindableRotation() { return &_rotationProp; }
    float health() const { return _healthProp.value(); }
    QBindable<float> bindableHealth() { return &_healthProp; }
    int side() const { return _sideProp.value(); }
    QBindable<int> bindableSide() { return &_sideProp; }
    float speed() const { return _speedProp.value(); }
    QBindable<float> bindableSpeed() { return &_speedProp; }

private:
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _xProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _yProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _rotationProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _healthProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, int, _sideProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _speedProp)
};

#endif  // OBJECTS_BASE_HPP
