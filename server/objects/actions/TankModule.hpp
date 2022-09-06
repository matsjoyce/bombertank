#ifndef OBJECTS_ACTIONS_TANK_MODULE_HPP
#define OBJECTS_ACTIONS_TANK_MODULE_HPP

#include "common/TcpMessageSocket.hpp"

class Game;
class TankState;

class TankModule {
    bool _active;
    int _reload = 0;

public:
    virtual ~TankModule() = default;
    virtual void prePhysics(Game* game, TankState* tank);
    virtual void postPhysics(Game* game, TankState* tank) {};
    virtual void act(Game* game, TankState* tank) {};
    virtual int type() = 0;
    void setActive(bool active) { _active=active; }
    bool active() const { return _active; }
    virtual float healthContribution() { return 0; }
    virtual float speedContribution() { return 0; }
    virtual float maxShieldContribution() { return 0; }
    virtual Message message();
    virtual float maxReload();
};

#endif // OBJECTS_ACTIONS_TANK_MODULE_HPP
