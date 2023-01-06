#include "DropModules.hpp"

#include <QDebug>

#include "../../Game.hpp"
#include "../Tank.hpp"
#include "common/Constants.hpp"

float CrateModule::maxReload() const {
    return 20;
}

void CrateModule::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    game->addObject(constants::ObjectType::CRATE, tank->body()->GetPosition() - tank->body()->GetWorldVector({3, 0}), 0, {0, 0});
}

float BombModule::maxReload() const {
    return 40;
}

void BombModule::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    game->addObject(constants::ObjectType::BOMB, tank->body()->GetPosition() - tank->body()->GetWorldVector({3, 0}), 0, {0, 0});
}

float TimedBombModule::maxReload() const {
    return 40;
}

void TimedBombModule::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    game->addObject(constants::ObjectType::TIMED_BOMB, tank->body()->GetPosition() - tank->body()->GetWorldVector({3, 0}), 0, {0, 0});
}

float MGTurretModule::maxReload() const {
    return 400;
}

void MGTurretModule::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    auto obj = game->addObject(constants::ObjectType::MG_TURRET, tank->body()->GetPosition() - tank->body()->GetWorldVector({3, 0}), 0, {0, 0});
    if (obj) {
        obj->second->setSide(tank->side());
    }
}

float MineModule::maxReload() const {
    return 60;
}

void MineModule::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    game->addObject(constants::ObjectType::MINE, tank->body()->GetPosition() - tank->body()->GetWorldVector({3, 0}), 0, {0, 0});
}
