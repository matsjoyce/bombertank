#include "RocketLauncher.hpp"

#include <QDebug>
#include <random>

#include "../../Game.hpp"
#include "../Tank.hpp"
#include "common/Constants.hpp"

void RocketLauncher::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    // Shoot
    qDebug() << "Create shell";
    auto forward = tank->turretVector();

    const auto speed = std::max(20.0f, b2Dot(tank->body()->GetLinearVelocity(), forward));
    auto velocity = speed * forward;
    auto obj = game->addObject(constants::ObjectType::ROCKET, tank->body()->GetPosition() + 3.75 * forward,
                                    std::atan2(velocity.y, velocity.x), velocity);
    if (obj) {
        obj->second->setSide(tank->side());
    }
}

void HomingRocketLauncher::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    // Shoot
    qDebug() << "Create shell";
    auto forward = tank->turretVector();

    const auto speed = std::max(20.0f, b2Dot(tank->body()->GetLinearVelocity(), forward));
    auto velocity = speed * forward;
    auto obj = game->addObject(constants::ObjectType::HOMING_ROCKET, tank->body()->GetPosition() + 3.75 * forward,
                                    std::atan2(velocity.y, velocity.x), velocity);
    if (obj) {
        obj->second->setSide(tank->side());
    }
}

float RocketLauncher::maxReload() {
    return 100;
}


