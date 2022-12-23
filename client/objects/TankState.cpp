#include "TankState.hpp"

void TankState::loadMessage(Message& msg) {
    BaseObjectState::loadMessage(msg);
    _shieldProp.setValue(msg["shield"].as_double());
    _turretAngleProp.setValue(msg["turretAngle"].as_double());
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
    int type = msg.at("type").is_uint64_t() ? msg.at("type").as_uint64_t() : msg.at("type").as_int64_t();
    _typeProp.setValue(type);
    if (type == -1) {
        _reloadProp.setValue(0);
        _usesProp.setValue(0);
        _pointsProp.setValue({});
    }
    else {
        _reloadProp.setValue(msg.at("reload").as_double());
        int beforeUses = _usesProp.value();
        _usesProp.setValue(msg.at("uses").as_uint64_t());
        if (beforeUses < _usesProp.value()) {
            emit used();
        }
        std::vector<QPointF> points;
        if (msg.count("points")) {
            for (auto p : msg["points"].as_vector()) {
                auto pv = p.as_vector();
                points.push_back({pv.at(0).as_double(), pv.at(1).as_double()});
            }
        }
        _pointsProp.setValue(points);
    }
}
