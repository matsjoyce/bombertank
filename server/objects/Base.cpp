#include "Base.hpp"

#include <QDebug>
#include <common/Constants.hpp>

const float IMPULSE_TO_DAMAGE = 10000.0f;
const float MIN_IMPULSE_DAMAGE = 1.0f;

BaseObjectState::BaseObjectState(int type) : _type(type) {}

void BaseObjectState::fillMessage(bt_messages::ToClientMessage_ObjectUpdated& msg) const {
    constants::StatusTypes status;
    if (stunned()) {
        status |= constants::STUNNED;
    }
    if (invisible()) {
        status |= constants::INVISIBLE;
    }

    msg.set_type(static_cast<uint64_t>(type()));
    msg.set_health(health() / maxHealth());
    msg.set_side(side());
    msg.set_x(_dead ? _deathPosition.x : body()->GetPosition().x);
    msg.set_y(_dead ? _deathPosition.y : body()->GetPosition().y);
    msg.set_rotation(_dead ? _deathAngle : body()->GetAngle());
    msg.set_vx(_dead ? 0 : body()->GetLinearVelocity().x);
    msg.set_vy(_dead ? 0 : body()->GetLinearVelocity().y);
    msg.set_status(status.toInt());
    msg.set_destroyed(_dead);
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

void BaseObjectState::handleMessage(const bt_messages::ToServerMessage_ControlState& msg) {}

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
