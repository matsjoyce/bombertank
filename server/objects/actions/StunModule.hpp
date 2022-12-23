#ifndef OBJECTS_ACTIONS_STUN_MODULE_HPP
#define OBJECTS_ACTIONS_STUN_MODULE_HPP

#include "TankModule.hpp"

class StunModule : public TankModule {
public:
    int type() override { return 10; }
    void act(Game* game, TankState* tank) override;
    float maxReload() override;
    float stunResistanceMultiplier() override;
};

#endif // OBJECTS_ACTIONS_STUN_MODULE_HPP
