#include "TankModule.hpp"

void TankModule::prePhysics(Game* game, TankState* tank) {
    if (_reload) {
        --_reload;
    }

    if (active() && !_reload) {
        act(game, tank);
        _reload = maxReload();
    }
}

float TankModule::maxReload() {
    return 0;
}

Message TankModule::message() {
    return {{"type", type()}, {"reload", _reload == maxReload() ? 0 : 1 - _reload / maxReload()}};
}

