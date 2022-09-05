#ifndef OBJECTS_ACTIONS_MAIN_GUN_HPP
#define OBJECTS_ACTIONS_MAIN_GUN_HPP

#include "TankModule.hpp"

class MainGun : public TankModule {
    int _reload = 0;

public:
    void prePhysics(Game* game, TankState* tank);
    float healthContribution() override;
};

#endif // OBJECTS_ACTIONS_MAIN_GUN_HPP
