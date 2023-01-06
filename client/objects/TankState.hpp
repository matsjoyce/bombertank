#ifndef OBJECTS_TANK_STATE_HPP
#define OBJECTS_TANK_STATE_HPP

#include "Base.hpp"
#include <QPointF>

class TankModuleState : public QObject {
    Q_OBJECT
    Q_PROPERTY(int type READ type BINDABLE bindableType)
    Q_PROPERTY(float reload READ reload BINDABLE bindableReload)
    Q_PROPERTY(int uses READ uses BINDABLE bindableUses)
    Q_PROPERTY(std::vector<QPointF> points READ points BINDABLE bindablePoints)
    QML_ELEMENT

   public:
    using QObject::QObject;
    virtual void loadMessage(const bt_messages::ToClientMessage_TankModuleUpdates& msg);

    int type() { return _typeProp.value(); }
    QBindable<int> bindableType() { return &_typeProp; }
    float reload() { return _reloadProp.value(); }
    QBindable<float> bindableReload() { return &_reloadProp; }
    int uses() { return _usesProp.value(); }
    QBindable<int> bindableUses() { return &_usesProp; }
    std::vector<QPointF> points() { return _pointsProp.value(); }
    QBindable<std::vector<QPointF>> bindablePoints() { return &_pointsProp; }

signals:
    void used();

private:
    Q_OBJECT_BINDABLE_PROPERTY(TankModuleState, int, _typeProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankModuleState, float, _reloadProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankModuleState, int, _usesProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankModuleState, std::vector<QPointF>, _pointsProp)
};

class TankState : public BaseObjectState {
    Q_OBJECT
    Q_PROPERTY(float shield READ shield BINDABLE bindableShield)
    Q_PROPERTY(float turretAngle READ turretAngle BINDABLE bindableTurretAngle)
    Q_PROPERTY(std::vector<TankModuleState*> modules READ modules BINDABLE bindableModules)
    Q_PROPERTY(float leftTrackMovement READ leftTrackMovement BINDABLE bindableLeftTrackMovement)
    Q_PROPERTY(float rightTrackMovement READ rightTrackMovement BINDABLE bindableRightTrackMovement)
    QML_ELEMENT

    std::vector<std::unique_ptr<TankModuleState>> _modulesPtrs;

public:
    float shield() { return _shieldProp.value(); }
    QBindable<float> bindableShield() { return &_shieldProp; }
    float turretAngle() const { return _turretAngleProp.value(); }
    QBindable<float> bindableTurretAngle() { return &_turretAngleProp; }
    const std::vector<TankModuleState*> modules() { return _modulesProp.value(); }
    QBindable<std::vector<TankModuleState*>> bindableModules() { return &_modulesProp; }
    float leftTrackMovement() const { return _leftTrackMovementProp.value(); }
    QBindable<float> bindableLeftTrackMovement() { return &_leftTrackMovementProp; }
    float rightTrackMovement() const { return _rightTrackMovementProp.value(); }
    QBindable<float> bindableRightTrackMovement() { return &_rightTrackMovementProp; }

    void loadMessage(const bt_messages::ToClientMessage_ObjectUpdated& msg) override;

private:
    Q_OBJECT_BINDABLE_PROPERTY(TankState, float, _shieldProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankState, float, _turretAngleProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankState, std::vector<TankModuleState*>, _modulesProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankState, float, _leftTrackMovementProp)
    Q_OBJECT_BINDABLE_PROPERTY(TankState, float, _rightTrackMovementProp)
};

#endif // OBJECTS_TANK_STATE_HPP
