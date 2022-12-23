#include "StunModule.hpp"
#include "../Tank.hpp"
#include "../Queries.hpp"

void StunModule::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    for (auto obj : queryObjectsInCircle(game, tank->body()->GetPosition(), 30)) {
        obj->stun(75);
    }
}

float StunModule::maxReload() {
    return 125;
}

float StunModule::stunResistanceMultiplier() {
    return 0;
}
