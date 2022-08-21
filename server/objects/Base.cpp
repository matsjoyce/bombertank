#include "Base.hpp"

#include <QDebug>

const float IMPULSE_TO_DAMAGE = 10000.0f;
const float MIN_IMPULSE_DAMAGE = 1.0f;

BaseObjectState::BaseObjectState() {}

Message BaseObjectState::message() const {
    return {
        {"type", static_cast<uint64_t>(type())},
        {"health", health() / maxHealth()},
        {"side", side()},
        {"x", _dead ? _deathPosition.x : body()->GetPosition().x},
        {"y", _dead ? _deathPosition.y : body()->GetPosition().y},
        {"rotation", _dead ? _deathAngle : body()->GetAngle()},
        {"vx", _dead ? 0 : body()->GetLinearVelocity().x},
        {"vy", _dead ? 0 : body()->GetLinearVelocity().y},
    };
}

void BaseObjectState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    _body = world.CreateBody(&bodyDef);
}

BaseObjectState::~BaseObjectState() { _body->GetUserData().pointer = 0; }

void BaseObjectState::prePhysics(Game* game) {}

void BaseObjectState::postPhysics(Game* game) {}

void BaseObjectState::handleMessage(const Message& msg) {}

void BaseObjectState::collision(BaseObjectState* other, float impulse) {}

std::pair<float, DamageType> BaseObjectState::impactDamage(float baseDamage) {
    return {baseDamage, DamageType::IMPACT};
}

void BaseObjectState::damage(float amount, DamageType type) {
    // qInfo() << "Took" << amount << "of damage";
    _damageTaken = std::min(maxHealth(), _damageTaken + amount);
    if (_damageTaken == maxHealth()) {
        _dead = true;
    }
}

void BaseObjectState::destroy(Game* game) {
    qInfo() << "Destroyed";
    _body->GetWorld()->DestroyBody(_body);
}

void BaseObjectState::die() {
    _dead = true;
    _deathPosition = body()->GetPosition();
    _deathAngle = body()->GetAngle();
}
