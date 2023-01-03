#ifndef OBJECTS_ACTIONS_STAT_BOOST_MODULES_HPP
#define OBJECTS_ACTIONS_STAT_BOOST_MODULES_HPP

#include "TankModule.hpp"

class HealthModule : public TankModule {
public:
    int type() override { return 3; }
    float healthContribution() override;
};


class SpeedModule : public TankModule {
public:
    int type() override { return 4; }
    float speedContribution() override;
};

class ShieldModule : public TankModule {
    int _timer = 0;
public:
    int type() override { return 5; }
    void prePhysics(Game * game, TankState * tank) override;
    float maxShieldContribution() override;
};

class InvisibiltyModule : public TankModule {
public:
    int type() override { return 11; }
    void act(Game* game, TankState* tank) override;
    float maxReload() override;
};

#endif // OBJECTS_ACTIONS_STAT_BOOST_MODULES_HPP

