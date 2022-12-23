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

float TankModule::maxReload() {
    return 0;
}

void TankModule::act(Game* game, TankState* tank) {
    ++_uses;
}

Message TankModule::message() {
    return {{"type", type()}, {"reload", _reload == maxReload() ? 0 : 1 - _reload / maxReload()}, {"uses", _uses}};
}

