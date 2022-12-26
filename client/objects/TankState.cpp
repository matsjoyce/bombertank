#include "TankState.hpp"
#include "common/MsgpackUtils.hpp"

void TankState::loadMessage(Message& msg) {
    BaseObjectState::loadMessage(msg);
    _shieldProp.setValue(extractDouble(msg["shield"]));
    _turretAngleProp.setValue(extractDouble(msg["turretAngle"]));
    _leftTrackMovementProp.setValue(extractDouble(msg["left_track_movement"]));
    _rightTrackMovementProp.setValue(extractDouble(msg["right_track_movement"]));
    auto modules = msg.at("modules").as_vector();
    bool updateBinding = false;
    while (_modulesPtrs.size() < modules.size()) {
        _modulesPtrs.emplace_back(std::make_unique<TankModuleState>());
        updateBinding = true;
    }
    while (_modulesPtrs.size() > modules.size()) {
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
    for (auto moduleMsg : modules) {
        auto moduleData = moduleMsg.as_multimap();
        Message convertedData;
        for (auto p : moduleData) {
            convertedData[p.first.as_string()] = p.second;
        }
        _modulesPtrs[idx]->loadMessage(convertedData);
        ++idx;
    }
}

void TankModuleState::loadMessage(Message& msg) {
    int type = extractInt(msg.at("type"));
    _typeProp.setValue(type);
    if (type == -1) {
        _reloadProp.setValue(0);
        _usesProp.setValue(0);
        _pointsProp.setValue({});
    }
    else {
        _reloadProp.setValue(extractDouble(msg.at("reload")));
        int beforeUses = _usesProp.value();
        _usesProp.setValue(msg.at("uses").as_uint64_t());
        if (beforeUses < _usesProp.value()) {
            emit used();
        }
        std::vector<QPointF> points;
        if (msg.count("points")) {
            for (auto p : msg["points"].as_vector()) {
                auto pv = p.as_vector();
                points.push_back({extractDouble(pv.at(0)), extractDouble(pv.at(1))});
            }
        }
        _pointsProp.setValue(points);
    }
}
