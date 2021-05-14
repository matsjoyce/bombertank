#include "Objects.hpp"

#include <functional>

#include "Crate.hpp"
#include "Projectiles.hpp"
#include "Tank.hpp"
#include "Walls.hpp"

std::unique_ptr<BaseObjectState> createObjectFromType(constants::ObjectType type) {
    switch (type) {
        case constants::ObjectType::TANK: {
            return std::make_unique<TankState>(type);
        }
        case constants::ObjectType::CRATE: {
            return std::make_unique<CrateState>(type);
        }
        case constants::ObjectType::WALL: {
            return std::make_unique<WallState>(type);
        }
        case constants::ObjectType::INDESTRUCTABLE_WALL: {
            return std::make_unique<IndestructableWallState>(type);
        }
        case constants::ObjectType::SHELL: {
            return std::make_unique<ShellState>(type);
        }
    }
    return {};
}