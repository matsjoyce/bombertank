#ifndef OBJECTS_ACTIONS_TANK_MODULE_HPP
#define OBJECTS_ACTIONS_TANK_MODULE_HPP

#include "common/TcpMessageSocket.hpp"

class Game;
class TankState;

class TankModule {
    bool _active = false;
    int _uses = 0;

protected:
    int _reload = 0;

public:
    virtual ~TankModule() = default;
    virtual void prePhysics(Game* game, TankState* tank);
    virtual void postPhysics(Game* game, TankState* tank) {};
    virtual void act(Game* game, TankState* tank);
    virtual int type() const = 0;
    void setActive(bool active) { _active = active; }
    bool active() const { return _active; }
    virtual float healthContribution() const { return 0; }
    virtual float speedContribution() const { return 0; }
    virtual float maxShieldContribution() const { return 0; }
    virtual float stunResistanceMultiplier() const { return 1; }
    virtual void fillMessage(bt_messages::ToClientMessage_TankModuleUpdates& msg) const;
    virtual float maxReload() const;
    virtual void reloaded() {};
};

#endif // OBJECTS_ACTIONS_TANK_MODULE_HPP
