#include "Base.hpp"

#include <QDebug>

const float IMPULSE_TO_DAMAGE = 10000.0f;
const float MIN_IMPULSE_DAMAGE = 2.0f;

Message BaseObjectState::message() const {
    return {{"type", static_cast<uint64_t>(type)}, {"health", health / maxHealth}};
}

void BaseObjectState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    body = world.CreateBody(&bodyDef);
}

BaseObjectState::~BaseObjectState() { body->GetUserData().pointer = 0; }

void BaseObjectState::prePhysics(Game* game) {}

void BaseObjectState::postPhysics(Game* game) {}

void BaseObjectState::handleMessage(const Message& msg) {}

void BaseObjectState::collision(BaseObjectState* other, float impulse) {
    // qInfo() << "Impulse of" << impulse;
    auto [damage_, type] = other->impactDamage(impactDamage(impulse / IMPULSE_TO_DAMAGE).first);
    if (damage_ >= MIN_IMPULSE_DAMAGE) {
        damage(damage_, type);
    }
}

std::pair<float, DamageType> BaseObjectState::impactDamage(float baseDamage) {
    return {baseDamage, DamageType::IMPACT};
}

void BaseObjectState::damage(float amount, DamageType type) {
    // qInfo() << "Took" << amount << "of damage";
    health = std::max(0.0f, health - amount);
    if (health == 0.0f) {
        dead = true;
    }
}

void BaseObjectState::destroy(Game* game) {
    qInfo() << "Destroyed";
    body->GetWorld()->DestroyBody(body);
}