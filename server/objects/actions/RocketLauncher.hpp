#ifndef OBJECTS_ACTIONS_ROCKET_LAUNCHER_HPP
#define OBJECTS_ACTIONS_ROCKET_LAUNCHER_HPP

#include "TankModule.hpp"

class RocketLauncher : public TankModule {
public:
    int type() override { return 1; }
    void act(Game* game, TankState* tank) override;
    float maxReload() override;
};

#endif // OBJECTS_ACTIONS_ROCKET_LAUNCHER_HPP


