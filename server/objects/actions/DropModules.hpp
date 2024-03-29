#ifndef OBJECTS_ACTIONS_DROP_MODULES_HPP
#define OBJECTS_ACTIONS_DROP_MODULES_HPP

#include "TankModule.hpp"

class CrateModule : public TankModule {
public:
    int type() const override { return 7; }
    float maxReload() const override;
    void act(Game* game, TankState* tank) override;
};

class BombModule : public TankModule {
public:
    int type() const override { return 8; }
    float maxReload() const override;
    void act(Game* game, TankState* tank) override;
};

class TimedBombModule : public TankModule {
public:
    int type() const override { return 12; }
    float maxReload() const override;
    void act(Game* game, TankState* tank) override;
};

class MGTurretModule : public TankModule {
public:
    int type() const override { return 13; }
    float maxReload() const override;
    void act(Game* game, TankState* tank) override;
};

class MineModule : public TankModule {
public:
    int type() const override { return 14; }
    float maxReload() const override;
    void act(Game* game, TankState* tank) override;
};

#endif // OBJECTS_ACTIONS_DROP_MODULES_HPP


