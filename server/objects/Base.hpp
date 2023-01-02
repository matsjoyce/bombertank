#ifndef OBJECTS_BASE_HPP
#define OBJECTS_BASE_HPP

#include <memory>

#include "box2d/box2d.h"
#include "common/Constants.hpp"
#include "common/TcpMessageSocket.hpp"

enum class DamageType { IMPACT, PIERCING, THERMAL };
enum class Hostility { NON_HOSTILE, VAGELY_HOSTILE, HOSTILE };

constexpr int SHELL_CATEGORY = 0x2;
constexpr int ROCKET_CATEGORY = 0x4;

class Game;

class BaseObjectState {
    int _type;
    bool _dead = false;
    b2Body* _body = nullptr;
    float _damageTaken = 0;
    int _side = 0;
    b2Vec2 _deathPosition;
    float _deathAngle;
    int _stunnedFor = 0, _invisibleFor = 0;

   public:
    BaseObjectState(int type);
    virtual ~BaseObjectState();
    int type() const { return _type; }
    virtual float maxHealth() const = 0;
    b2Body* body() const { return _body; }
    float health() const { return maxHealth() - _damageTaken; }
    bool dead() const { return _dead; }
    bool stunned() const { return _stunnedFor != 0; }
    bool invisible() const { return _invisibleFor != 0; }
    int side() const { return _side; }
    void setSide(int side) { _side = side; }
    virtual Message message() const;
    virtual void createBodies(Game* game, b2World& world, b2BodyDef& bodyDef);
    virtual void prePhysics(Game* game);
    virtual void postPhysics(Game* game);
    virtual void handleMessage(const Message& msg);
    virtual void collision(BaseObjectState* other, float impulse);
    virtual void damage(float amount, DamageType type);
    virtual void stun(int amount);
    virtual void invisiblize(int amount);
    virtual void destroy(Game* game);
    virtual std::pair<float, DamageType> impactDamage(float baseDamage);
    virtual Hostility hostility() const;
    void die();
};

typedef std::unique_ptr<BaseObjectState>(*ObjectStateFn)(int type);

class ObjectStateRegister {
    static std::map<std::string, ObjectStateFn>& _registry();
public:
    template<class T> static int registerObjectState(std::string name) {
        _registry()[name] = +[](int type) -> std::unique_ptr<BaseObjectState>{ return std::make_unique<T>(type); };
        return 0;
    }
    static std::unique_ptr<BaseObjectState> createObject(std::string name, int type);
    static void dumpRegistry();
};

#define REGISTER_STATE(cls) static const int _##cls##Registration = ObjectStateRegister::registerObjectState<cls>(#cls);

#endif  // OBJECTS_BASE_HPP
