#include "TankState.hpp"
#include "common/MsgpackUtils.hpp"

void TankState::loadMessage(const bt_messages::ToClientMessage_ObjectUpdated& msg) {
    BaseObjectState::loadMessage(msg);
    auto& tank_updates = msg.tank_updates();
    _shieldProp.setValue(tank_updates.shield());
    _turretAngleProp.setValue(msg.turret_angle());
    _leftTrackMovementProp.setValue(tank_updates.left_track_movement());
    _rightTrackMovementProp.setValue(tank_updates.right_track_movement());
    bool updateBinding = false;
    while (_modulesPtrs.size() < tank_updates.modules().size()) {
        _modulesPtrs.emplace_back(std::make_unique<TankModuleState>());
        updateBinding = true;
    }
    while (_modulesPtrs.size() > tank_updates.modules().size()) {
        _modulesPtrs.pop_back();
        updateBinding = true;
    }
    if (updateBinding) {
        std::vector<TankModuleState*> ptrs;
        for (auto& mod : _modulesPtrs) {
            ptrs.push_back(mod.get());
        }
        _modulesProp.setValue(ptrs);
    }
    int idx = 0;
    for (auto& module_ : tank_updates.modules()) {
        _modulesPtrs[idx++]->loadMessage(module_);
    }
}

void TankModuleState::loadMessage(const bt_messages::ToClientMessage_TankModuleUpdates& msg) {
    _typeProp.setValue(msg.type());
    if (msg.type() == -1) {
        _reloadProp.setValue(0);
        _usesProp.setValue(0);
        _pointsProp.setValue({});
    }
    else {
        _reloadProp.setValue(extractDouble(msg.reload()));
        int beforeUses = _usesProp.value();
        _usesProp.setValue(msg.uses());
        if (beforeUses < _usesProp.value()) {
            emit used();
        }
        std::vector<QPointF> points;
        for (auto p : msg.points()) {
            points.push_back({p.x(), p.y()});
        }
        _pointsProp.setValue(points);
    }
}
