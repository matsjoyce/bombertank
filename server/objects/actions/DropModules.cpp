#include "DropModules.hpp"

#include <QDebug>

#include "../../Game.hpp"
#include "../Tank.hpp"
#include "common/Constants.hpp"

float CrateModule::maxReload() {
    return 20;
}

void CrateModule::act(Game* game, TankState* tank) {
    game->addObject(constants::ObjectType::CRATE, tank->body()->GetPosition() - tank->body()->GetWorldVector({3, 0}), 0, {0, 0});
}

float BombModule::maxReload() {
    return 40;
}

void BombModule::act(Game* game, TankState* tank) {
    game->addObject(constants::ObjectType::BOMB, tank->body()->GetPosition() - tank->body()->GetWorldVector({3, 0}), 0, {0, 0});
}