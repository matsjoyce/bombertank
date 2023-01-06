#include "TankModule.hpp"

void TankModule::prePhysics(Game* game, TankState* tank) {
    if (_reload) {
        --_reload;
        if (!_reload) {
            reloaded();
        }
    }

    if (active() && !_reload) {
        act(game, tank);
        _reload = maxReload();
    }
}

float TankModule::maxReload() const {
    return 0;
}

void TankModule::act(Game* game, TankState* tank) {
    ++_uses;
}

void TankModule::fillMessage(bt_messages::ToClientMessage_TankModuleUpdates& msg) const {
    msg.set_type(type());
    msg.set_reload(_reload == maxReload() ? 0 : 1 - _reload / maxReload());
    msg.set_uses(_uses);
}

