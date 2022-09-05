#include "StatBoostModules.hpp"

#include <QDebug>
#include <random>

#include "../Tank.hpp"
#include "common/Constants.hpp"

float HealthModule::healthContribution() {
    return 50;
}

float SpeedModule::speedContribution() {
    return 20;
}

void ShieldModule::prePhysics(Game* game, TankState* tank) {
    tank->addShield(-1);
    if (_reload) {
        --_reload;
    }

    if (active() && !_reload) {
        tank->addShield(maxShieldContribution());
        _reload = 100;
    }
}

float ShieldModule::maxShieldContribution() {
    return 100;
}
