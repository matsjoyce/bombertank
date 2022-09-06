#ifndef OBJECTS_ACTIONS_MACHINE_GUN_HPP
#define OBJECTS_ACTIONS_MACHINE_GUN_HPP

#include "TankModule.hpp"

class MachineGun : public TankModule {
public:
    int type() override { return 2; }
    void act(Game* game, TankState* tank) override;
    float maxReload() override;
};

#endif // OBJECTS_ACTIONS_MACHINE_GUN_HPP

