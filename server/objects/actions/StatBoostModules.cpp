#include "StatBoostModules.hpp"

#include <QDebug>
#include <random>

#include "../Tank.hpp"
#include "common/Constants.hpp"

float HealthModule::healthContribution() const {
    return 50;
}

float SpeedModule::speedContribution() const {
    return 10;
}

void ShieldModule::prePhysics(Game* game, TankState* tank) {
    TankModule::prePhysics(game, tank);
    ++_timer;
    if (_timer >= 10) {
        _timer = 0;
        tank->addShield(1);
    }
}

float ShieldModule::maxShieldContribution() const {
    return 25;
}

void InvisibiltyModule::act(Game* game, TankState* tank) {
    tank->invisiblize(100);
}

float InvisibiltyModule::maxReload() const {
    return 200;
}
