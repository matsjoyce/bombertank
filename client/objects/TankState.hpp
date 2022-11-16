#ifndef OBJECTS_TANK_STATE_HPP
#define OBJECTS_TANK_STATE_HPP

#include "Base.hpp"

class TankModuleState : public QObject {
    Q_OBJECT
    Q_PROPERTY(int type READ type BINDABLE bindableType)
    Q_PROPERTY(float reload READ reload BINDABLE bindableReload)
    QML_ELEMENT

   public:
    using QObject::QObject;
    virtual void loadMessage(Message& msg);

    int type() { return _typeProp.value(); }
    QBindable<int> bindableType() { return &_typeProp; }
    float reload() { return _reloadProp.value(); }
    QBindable<float> bindableReload() { return &_reloadProp; }

private:
    Q_OBJECT_BINDABLE_PROPERTY(TankModuleState, int, _typeProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankModuleState, float, _reloadProp)
};

class TankState : public BaseObjectState {
    Q_OBJECT
    Q_PROPERTY(float shield READ shield BINDABLE bindableShield)
    Q_PROPERTY(float turretAngle READ turretAngle BINDABLE bindableTurretAngle)
    Q_PROPERTY(std::vector<TankModuleState*> modules READ modules BINDABLE bindableModules)
    QML_ELEMENT

    std::vector<std::unique_ptr<TankModuleState>> _modulesPtrs;

public:
    float shield() { return _shieldProp.value(); }
    QBindable<float> bindableShield() { return &_shieldProp; }
    float turretAngle() const { return _turretAngleProp.value(); }
    QBindable<float> bindableTurretAngle() { return &_turretAngleProp; }
    const std::vector<TankModuleState*> modules() { return _modulesProp.value(); }
    QBindable<std::vector<TankModuleState*>> bindableModules() { return &_modulesProp; }
    void loadMessage(Message& msg) override;

private:
    Q_OBJECT_BINDABLE_PROPERTY(TankState, float, _shieldProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankState, float, _turretAngleProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankState, std::vector<TankModuleState*>, _modulesProp)
};

#endif // OBJECTS_TANK_STATE_HPP
