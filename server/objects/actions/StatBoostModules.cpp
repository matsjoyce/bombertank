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
//     tank->addShield(-1);
    TankModule::prePhysics(game, tank);
}

void ShieldModule::act(Game* game, TankState* tank) {
    tank->addShield(maxShieldContribution());
}

float ShieldModule::maxReload() {
    return 100;
}

float ShieldModule::maxShieldContribution() {
    return 100;
}
