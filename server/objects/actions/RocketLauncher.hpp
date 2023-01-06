#ifndef OBJECTS_ACTIONS_ROCKET_LAUNCHER_HPP
#define OBJECTS_ACTIONS_ROCKET_LAUNCHER_HPP

#include "TankModule.hpp"

class RocketLauncher : public TankModule {
public:
    int type() const override { return 1; }
    void act(Game* game, TankState* tank) override;
    float maxReload() const override;
};

class HomingRocketLauncher : public RocketLauncher {
public:
    int type() const override { return 9; }
    void act(Game* game, TankState* tank) override;
    float maxReload() const override;
};

#endif // OBJECTS_ACTIONS_ROCKET_LAUNCHER_HPP


