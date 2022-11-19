#include "MachineGun.hpp"

#include <QDebug>
#include <random>

#include "../../Game.hpp"
#include "../Tank.hpp"
#include "../Projectiles.hpp"
#include "common/Constants.hpp"

void MachineGun::act(Game* game, TankState* tank) {
    // Shoot
    qDebug() << "Create shell";
    auto forward = tank->turretVector();
    auto sideways = b2Vec2{forward.y, -forward.x};

    const auto speed = 100;
    const auto maxSidewaysVelocity = speed / 20;
    std::uniform_real_distribution<float> distribution(-maxSidewaysVelocity, maxSidewaysVelocity);
    auto velocity = speed * forward + distribution(game->randomGenerator()) * sideways;
    auto pos = tank->body()->GetPosition() + 3.5 * forward + 0.5 * sideways;
    qDebug() << (tank->body()->GetPosition() - pos).Length();
    auto shell = game->addObject(constants::ObjectType::MG_SHELL, tank->body()->GetPosition() + 3.5 * forward + 0.5 * sideways,
                                    std::atan2(velocity.y, velocity.x), velocity);
    if (shell) {
        tank->body()->ApplyLinearImpulseToCenter(-velocity.Length() * shell->second->body()->GetMass() / 2 * forward, true);
    }
}

float MachineGun::maxReload() {
    return 2;
}

float Laser::maxReload() {
    return 1;
}

void Laser::prePhysics(Game* game, TankState* tank) {
    if (!_laser) {
        auto obj = game->addObject(constants::ObjectType::LASER, tank->body()->GetPosition(), 0, {0, 0});
        if (obj) {
            _laser = dynamic_cast<LaserState*>(obj->second);
        }
    }
    if (_energy < 20) {
        ++_energy;
    }
    else {
        _reload = 0;
    }
    _laser->setMaxLength(0);

    if (active()) {
        auto forward = tank->turretVector();
        _laser->body()->SetTransform(tank->body()->GetPosition() + 3 * forward, tank->turretAngle());
        if (_energy > 2 && !_reload) {
            _laser->setMaxLength(100);
            _energy -= 2;
            return;
        }
        else {
            _reload = 1;
        }
    }
}

void Laser::postPhysics(Game* game, TankState* tank) {
    if (tank->dead() && _laser) {
        _laser->die();
    }
}
