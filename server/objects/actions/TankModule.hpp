#ifndef OBJECTS_ACTIONS_TANK_MODULE_HPP
#define OBJECTS_ACTIONS_TANK_MODULE_HPP

class Game;
class TankState;

class TankModule {
    bool _active;

public:
    virtual ~TankModule() = default;
    virtual void prePhysics(Game* game, TankState* tank) {};
    virtual void postPhysics(Game* game, TankState* tank) {};
    void setActive(bool active) { _active=active; }
    bool active() const { return _active; }
    virtual float healthContribution() { return 0; }
    virtual float speedContribution() { return 0; }
    virtual float maxShieldContribution() { return 0; }
};

#endif // OBJECTS_ACTIONS_TANK_MODULE_HPP
