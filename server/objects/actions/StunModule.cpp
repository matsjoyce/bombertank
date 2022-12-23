#include "StunModule.hpp"
#include "../Tank.hpp"
#include "../../Game.hpp"
#include "../Queries.hpp"

void StunModule::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    game->addObject(constants::ObjectType::STUN_WAVE, tank->body()->GetPosition(), 0, {0, 0});
}

float StunModule::maxReload() {
    return 125;
}

float StunModule::stunResistanceMultiplier() {
    return 0;
}
