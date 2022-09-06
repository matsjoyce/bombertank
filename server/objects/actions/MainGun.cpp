#include "MainGun.hpp"

#include <QDebug>
#include <random>

#include "../../Game.hpp"
#include "../Tank.hpp"
#include "common/Constants.hpp"

void MainGun::act(Game* game, TankState* tank) {
    // Shoot
    qDebug() << "Create shell";
    auto forward = tank->body()->GetWorldVector({1, 0});
    auto sideways = tank->body()->GetWorldVector({0, 1});

    const auto speed = 200;
    const auto maxSidewaysVelocity = speed / 50;
    std::uniform_real_distribution<float> distribution(-maxSidewaysVelocity, maxSidewaysVelocity);
    auto velocity = speed * forward + distribution(game->randomGenerator()) * sideways;
    auto shell = game->addObject(constants::ObjectType::SHELL, tank->body()->GetPosition() + 3.5 * forward,
                                    std::atan2(velocity.y, velocity.x), velocity);
    if (shell) {
        tank->body()->ApplyLinearImpulseToCenter(-velocity.Length() * shell->second->body()->GetMass() * forward, true);
    }
}

float MainGun::maxReload() {
    return 15;
}


float MainGun::healthContribution() {
    return 50;
}
