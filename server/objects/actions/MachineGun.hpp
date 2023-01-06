#ifndef OBJECTS_ACTIONS_MACHINE_GUN_HPP
#define OBJECTS_ACTIONS_MACHINE_GUN_HPP

#include "TankModule.hpp"

class LaserState;

class MachineGun : public TankModule {
public:
    int type() const override { return 2; }
    void act(Game* game, TankState* tank) override;
    float maxReload() const override;
};

class Laser : public TankModule {
    LaserState* _laser = nullptr;
    int _energy = 0;
public:
    int type() const override { return 6; }
    void prePhysics(Game* game, TankState* tank) override;
    void postPhysics(Game* game, TankState* tank) override;
    float maxReload() const override;
};

#endif // OBJECTS_ACTIONS_MACHINE_GUN_HPP

