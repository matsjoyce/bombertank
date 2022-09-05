#ifndef OBJECTS_ACTIONS_STAT_BOOST_MODULES_HPP
#define OBJECTS_ACTIONS_STAT_BOOST_MODULES_HPP

#include "TankModule.hpp"

class HealthModule : public TankModule {
public:
    float healthContribution() override;
};


class SpeedModule : public TankModule {
public:
    float speedContribution() override;
};

class ShieldModule : public TankModule {
    int _reload = 0;

public:
    void prePhysics(Game* game, TankState* tank);
    float maxShieldContribution() override;
};

#endif // OBJECTS_ACTIONS_STAT_BOOST_MODULES_HPP

