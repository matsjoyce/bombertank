#include "StatBoostModules.hpp"

#include <QDebug>
#include <random>

#include "../Tank.hpp"
#include "common/Constants.hpp"

float HealthModule::healthContribution() {
    return 50;
}

float SpeedModule::speedContribution() {
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

float ShieldModule::maxShieldContribution() {
    return 25;
}

void InvisibiltyModule::act(Game* game, TankState* tank) {
    tank->invisiblize(100);
}

float InvisibiltyModule::maxReload() {
    return 200;
}
