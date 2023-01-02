#include "Base.hpp"

#include <QDebug>
#include <common/Constants.hpp>

const float IMPULSE_TO_DAMAGE = 10000.0f;
const float MIN_IMPULSE_DAMAGE = 1.0f;

BaseObjectState::BaseObjectState(int type) : _type(type) {}

Message BaseObjectState::message() const {
    constants::StatusTypes status;
    if (stunned()) {
        status |= constants::STUNNED;
    }
    if (invisible()) {
        status |= constants::INVISIBLE;
    }
    return {
        {"type", static_cast<uint64_t>(type())},
        {"health", health() / maxHealth()},
        {"side", side()},
        {"x", _dead ? _deathPosition.x : body()->GetPosition().x},
        {"y", _dead ? _deathPosition.y : body()->GetPosition().y},
        {"rotation", _dead ? _deathAngle : body()->GetAngle()},
        {"vx", _dead ? 0 : body()->GetLinearVelocity().x},
        {"vy", _dead ? 0 : body()->GetLinearVelocity().y},
        {"status", status.toInt()}
    };
}

void BaseObjectState::createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) {
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    _body = world.CreateBody(&bodyDef);
}

BaseObjectState::~BaseObjectState() { _body->GetUserData().pointer = 0; }

void BaseObjectState::prePhysics(Game* game) {
    if (stunned()) {
        --_stunnedFor;
    }
    if (invisible()) {
        --_invisibleFor;
    }
}

void BaseObjectState::postPhysics(Game* game) {}

void BaseObjectState::handleMessage(const Message& msg) {}

void BaseObjectState::collision(BaseObjectState* other, float impulse) {}

std::pair<float, DamageType> BaseObjectState::impactDamage(float baseDamage) {
    return {baseDamage, DamageType::IMPACT};
}

void BaseObjectState::damage(float amount, DamageType type) {
    _damageTaken = std::min(maxHealth(), _damageTaken + amount);
    if (_damageTaken == maxHealth()) {
        die();
    }
}

void BaseObjectState::stun(int amount) {
    _stunnedFor += amount;
}

void BaseObjectState::invisiblize(int amount) {
    _invisibleFor += amount;
}


void BaseObjectState::destroy(Game* game) {
    _body->GetWorld()->DestroyBody(_body);
}

void BaseObjectState::die() {
    _dead = true;
    _deathPosition = body()->GetPosition();
    _deathAngle = body()->GetAngle();
}

Hostility BaseObjectState::hostility() const {
    return Hostility::NON_HOSTILE;
}

std::map<std::string, ObjectStateFn>& ObjectStateRegister::_registry() {
    static std::map<std::string, ObjectStateFn> r;
    return r;
}

std::unique_ptr<BaseObjectState> ObjectStateRegister::createObject(std::string name, int type) {
    if (!_registry().count(name)) {
        return {};
    }
    ObjectStateFn fn = _registry()[name];
    return fn(type);
}

void ObjectStateRegister::dumpRegistry() {
    auto dbg = qDebug() << "State registry" << _registry().size() << "list:";
    for (auto& [name, _] : _registry()) {
        dbg << QString::fromStdString(name);
    }
}
