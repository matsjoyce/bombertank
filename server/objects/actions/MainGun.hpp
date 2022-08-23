#ifndef OBJECTS_ACTIONS_MAIN_GUN_HPP
#define OBJECTS_ACTIONS_MAIN_GUN_HPP

#include "TankModule.hpp"

class MainGun : public TankModule {
    bool _active;
    int _reload = 0;

public:
    void prePhysics(Game* game, BaseObjectState* tank);
};

#endif // OBJECTS_ACTIONS_MAIN_GUN_HPP
