#include "Objects.hpp"

#include <functional>

#include "Crate.hpp"
#include "Projectiles.hpp"
#include "Tank.hpp"
#include "Walls.hpp"
#include "Markers.hpp"
#include "Turret.hpp"

std::unique_ptr<BaseObjectState> createObjectFromType(constants::ObjectType type) {
    switch (type) {
        case constants::ObjectType::TANK: {
            return std::make_unique<TankState>();
        }
        case constants::ObjectType::CRATE: {
            return std::make_unique<CrateState>();
        }
        case constants::ObjectType::WALL: {
            return std::make_unique<WallState>();
        }
        case constants::ObjectType::INDESTRUCTABLE_WALL: {
            return std::make_unique<IndestructableWallState>();
        }
        case constants::ObjectType::SHELL: {
            return std::make_unique<ShellState>();
        }
        case constants::ObjectType::MG_SHELL: {
            return std::make_unique<MGShellState>();
        }
        case constants::ObjectType::ROCKET: {
            return std::make_unique<RocketState>();
        }
        case constants::ObjectType::START_ZONE: {
            return std::make_unique<StartZoneState>();
        }
        case constants::ObjectType::EXPLOSION: {
            return std::make_unique<ExplosionState>();
        }
        case constants::ObjectType::BOMB: {
            return std::make_unique<BombState>();
        }
        case constants::ObjectType::LASER_TURRET: {
            return std::make_unique<LaserTurretState>();
        }
        case constants::ObjectType::MG_TURRET: {
            return std::make_unique<MachineGunTurretState>();
        }
        case constants::ObjectType::LASER: {
            return std::make_unique<LaserState>();
        }
        case constants::ObjectType::HOMING_ROCKET: {
            return std::make_unique<HomingRocketState>();
        }
        case constants::ObjectType::STUN_WAVE: {
            return std::make_unique<StunWaveState>();
        }
    }
    return {};
}
