#ifndef OBJECTS_ACTIONS_TANK_MODULE_HPP
#define OBJECTS_ACTIONS_TANK_MODULE_HPP

class Game;
class BaseObjectState;

class TankModule {
    bool _active;

public:
    virtual ~TankModule() = default;
    virtual void prePhysics(Game* game, BaseObjectState* tank) {};
    virtual void postPhysics(Game* game, BaseObjectState* tank) {};
    void setActive(bool active) { _active=active; }
    bool active() const { return _active; }
};

#endif // OBJECTS_ACTIONS_TANK_MODULE_HPP
