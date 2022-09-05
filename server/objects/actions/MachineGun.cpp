#include "MachineGun.hpp"

#include <QDebug>
#include <random>

#include "../../Game.hpp"
#include "../Tank.hpp"
#include "common/Constants.hpp"

void MachineGun::prePhysics(Game* game, TankState* tank) {
    if (_reload) {
        --_reload;
    }

    if (active() && !_reload) {
        // Shoot
        qDebug() << "Create shell";
        auto forward = tank->body()->GetWorldVector({1, 0});
        auto sideways = tank->body()->GetWorldVector({0, 1});

        const auto speed = 100;
        const auto maxSidewaysVelocity = speed / 20;
        std::uniform_real_distribution<float> distribution(-maxSidewaysVelocity, maxSidewaysVelocity);
        auto velocity = speed * forward + distribution(game->randomGenerator()) * sideways;
        auto shell = game->addObject(constants::ObjectType::MG_SHELL, tank->body()->GetPosition() + 3.5 * forward + 1 * sideways,
                                     std::atan2(velocity.y, velocity.x), velocity);
        if (shell) {
            tank->body()->ApplyLinearImpulseToCenter(-velocity.Length() * shell->second->body()->GetMass() * forward, true);
        }
        _reload = 2;
    }
}

