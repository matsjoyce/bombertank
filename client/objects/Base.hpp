#ifndef OBJECTS_BASE_HPP
#define OBJECTS_BASE_HPP

#include <QObject>
#include <QProperty>
#include <QQmlEngine>

#include "common/Constants.hpp"
#include "common/TcpMessageSocket.hpp"

class BaseObjectState : public QObject {
    Q_OBJECT
    Q_PROPERTY(int type READ type BINDABLE bindableType)
    Q_PROPERTY(float x READ x BINDABLE bindableX)
    Q_PROPERTY(float y READ y BINDABLE bindableY)
    Q_PROPERTY(float rotation READ rotation BINDABLE bindableRotation)
    Q_PROPERTY(float health READ health BINDABLE bindableHealth)
    Q_PROPERTY(int side READ side BINDABLE bindableSide)
    Q_PROPERTY(float speed READ speed BINDABLE bindableSpeed)
    Q_PROPERTY(bool destroyed READ destroyed BINDABLE bindableDestroyed)
    Q_PROPERTY(bool controlled READ controlled BINDABLE bindableControlled)
    Q_PROPERTY(int status READ status BINDABLE bindableStatus)
    QML_ELEMENT

   public:
    using QObject::QObject;
    virtual void loadMessage(const bt_messages::ToClientMessage_ObjectUpdated& msg);
    void setFromEditor(constants::ObjectType type, float x, float y, float rotation);

    int type() const { return _typeProp.value(); }
    QBindable<int> bindableType() { return &_typeProp; }
    float x() const { return _xProp.value(); }
    QBindable<float> bindableX() { return &_xProp; }
    float y() const { return _yProp.value(); }
    QBindable<float> bindableY() { return &_yProp; }
    float rotation() const { return _rotationProp.value(); }
    QBindable<float> bindableRotation() { return &_rotationProp; }
    float health() const { return _healthProp.value(); }
    QBindable<float> bindableHealth() { return &_healthProp; }
    int side() const { return _sideProp.value(); }
    QBindable<int> bindableSide() { return &_sideProp; }
    float speed() const { return _speedProp.value(); }
    QBindable<float> bindableSpeed() { return &_speedProp; }
    bool destroyed() const { return _destroyedProp.value(); }
    QBindable<bool> bindableDestroyed() { return &_destroyedProp; }
    int status() const { return _statusProp.value(); }
    QBindable<int> bindableStatus() { return &_statusProp; }
    bool controlled() const { return _controlledProp.value(); }
    void setControlled(bool value) { _controlledProp.setValue(value); }
    QBindable<bool> bindableControlled() { return &_controlledProp; }

private:
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, int, _typeProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _xProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _yProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _rotationProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _healthProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, int, _sideProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, float, _speedProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, bool, _destroyedProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, int, _statusProp)
    Q_OBJECT_BINDABLE_PROPERTY(BaseObjectState, bool, _controlledProp)
};

#endif  // OBJECTS_BASE_HPP
