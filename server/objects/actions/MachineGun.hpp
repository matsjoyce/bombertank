#ifndef OBJECTS_ACTIONS_MACHINE_GUN_HPP
#define OBJECTS_ACTIONS_MACHINE_GUN_HPP

#include "TankModule.hpp"

class MachineGun : public TankModule {
    bool _active;
    int _reload = 0;

public:
    void prePhysics(Game* game, BaseObjectState* tank);
};

#endif // OBJECTS_ACTIONS_MACHINE_GUN_HPP

