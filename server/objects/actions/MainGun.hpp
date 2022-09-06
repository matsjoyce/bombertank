#ifndef OBJECTS_ACTIONS_MAIN_GUN_HPP
#define OBJECTS_ACTIONS_MAIN_GUN_HPP

#include "TankModule.hpp"

class MainGun : public TankModule {
public:
    int type() override { return 0; }
    void act(Game* game, TankState* tank) override;
    float maxReload() override;
    float healthContribution() override;
};

#endif // OBJECTS_ACTIONS_MAIN_GUN_HPP
