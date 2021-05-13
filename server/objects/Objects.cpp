#include "Objects.hpp"

#include "Crate.hpp"
#include "Projectiles.hpp"
#include "Tank.hpp"
#include "Walls.hpp"

std::map<int, std::function<std::unique_ptr<BaseObjectState>()>> constructorForObject = {
    {1, []() { return std::make_unique<TankState>(); }},
    {2, []() { return std::make_unique<CrateState>(); }},
    {3, []() { return std::make_unique<WallState>(); }},
    {4, []() { return std::make_unique<IndestructableWallState>(); }},
    {5, []() { return std::make_unique<ShellState>(); }}};

std::unique_ptr<BaseObjectState> createObjectFromType(int type) {
    auto iter = constructorForObject.find(type);
    if (iter == constructorForObject.end()) {
        return {};
    }
    return iter->second();
}