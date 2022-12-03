#include "RocketLauncher.hpp"

#include <QDebug>
#include <random>

#include "../../Game.hpp"
#include "../Tank.hpp"
#include "common/Constants.hpp"

void RocketLauncher::act(Game* game, TankState* tank) {
    // Shoot
    qDebug() << "Create shell";
    auto forward = tank->turretVector();

    const auto speed = 20;
    auto velocity = speed * forward + tank->body()->GetLinearVelocity();
    auto obj = game->addObject(constants::ObjectType::ROCKET, tank->body()->GetPosition() + 3.5 * forward,
                                    std::atan2(velocity.y, velocity.x), velocity);
    if (obj) {
        obj->second->setSide(tank->side());
    }
}

void HomingRocketLauncher::act(Game* game, TankState* tank) {
    // Shoot
    qDebug() << "Create shell";
    auto forward = tank->turretVector();

    const auto speed = 20;
    auto velocity = speed * forward + tank->body()->GetLinearVelocity();
    auto obj = game->addObject(constants::ObjectType::HOMING_ROCKET, tank->body()->GetPosition() + 3.5 * forward,
                                    std::atan2(velocity.y, velocity.x), velocity);
    if (obj) {
        obj->second->setSide(tank->side());
    }
}

float RocketLauncher::maxReload() {
    return 100;
}


