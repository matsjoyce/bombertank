#include "RocketLauncher.hpp"

#include <QDebug>
#include <random>

#include "../../Game.hpp"
#include "../Tank.hpp"
#include "common/Constants.hpp"

void RocketLauncher::act(Game* game, TankState* tank) {
    // Shoot
    qDebug() << "Create shell";
    auto forward = tank->body()->GetWorldVector({1, 0});

    const auto speed = 20;
    auto velocity = speed * forward + tank->body()->GetLinearVelocity();
    game->addObject(constants::ObjectType::ROCKET, tank->body()->GetPosition() + 3.5 * forward,
                                    std::atan2(velocity.y, velocity.x), velocity);
}

float RocketLauncher::maxReload() {
    return 100;
}


