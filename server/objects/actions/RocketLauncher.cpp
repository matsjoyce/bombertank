#include "RocketLauncher.hpp"

#include <QDebug>
#include <random>

#include "../../Game.hpp"
#include "../Tank.hpp"
#include "common/Constants.hpp"

void RocketLauncher::prePhysics(Game* game, TankState* tank) {
    if (_reload) {
        --_reload;
    }

    if (active() && !_reload) {
        // Shoot
        qDebug() << "Create shell";
        auto forward = tank->body()->GetWorldVector({1, 0});

        const auto speed = 20;
        auto velocity = speed * forward + tank->body()->GetLinearVelocity();
        game->addObject(constants::ObjectType::ROCKET, tank->body()->GetPosition() + 3.5 * forward,
                                     std::atan2(velocity.y, velocity.x), velocity);
        _reload = 30;
    }
}


