#include "Objects.hpp"

#include <functional>

#include "Crate.hpp"
#include "Projectiles.hpp"
#include "Tank.hpp"
#include "Walls.hpp"

std::unique_ptr<BaseObjectState> createObjectFromType(ObjectType type) {
    switch (type) {
        case ObjectType::TANK: {
            return std::make_unique<TankState>(type);
        }
        case ObjectType::CRATE: {
            return std::make_unique<CrateState>(type);
        }
        case ObjectType::WALL: {
            return std::make_unique<WallState>(type);
        }
        case ObjectType::INDESTRUCTABLE_WALL: {
            return std::make_unique<IndestructableWallState>(type);
        }
        case ObjectType::SHELL: {
            return std::make_unique<ShellState>(type);
        }
    }
    return {};
}