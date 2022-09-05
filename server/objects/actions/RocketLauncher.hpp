#ifndef OBJECTS_ACTIONS_ROCKET_LAUNCHER_HPP
#define OBJECTS_ACTIONS_ROCKET_LAUNCHER_HPP

#include "TankModule.hpp"

class RocketLauncher : public TankModule {
    int _reload = 0;

public:
    void prePhysics(Game* game, TankState* tank);
};

#endif // OBJECTS_ACTIONS_ROCKET_LAUNCHER_HPP


