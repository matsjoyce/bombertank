#include "Projectiles.hpp"
#include "../Game.hpp"

const float IMPULSE_TO_DAMAGE = 1000.0f;

float ShellState::maxHealth() const { return 2; }

void ShellState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;

    BaseObjectState::createBodies(world, bodyDef);

    b2CircleShape circ;
    circ.m_radius = _bodyRadius();

    body()->CreateFixture(&circ, 100.0);
}

void ShellState::prePhysics(Game* game) {
    --_selfDestruct;
    if (_selfDestruct < 0) {
        die();
    }
}

std::pair<float, DamageType> ShellState::impactDamage(float baseDamage) {
    return {baseDamage * 100, DamageType::PIERCING};
}


void ShellState::collision(BaseObjectState* other, float impulse) {
    other->damage(impulse / IMPULSE_TO_DAMAGE * 3, DamageType::PIERCING);
    die();
}


void MGShellState::collision(BaseObjectState* other, float impulse) {
    other->damage(impulse / IMPULSE_TO_DAMAGE, DamageType::PIERCING);
    die();
}


void RocketState::prePhysics(Game* game) {
    ShellState::prePhysics(game);
    body()->ApplyLinearImpulseToCenter(body()->GetMass() * 10 * body()->GetWorldVector({1, 0}), true);
}

void RocketState::destroy(Game* game) {
    game->addObject(constants::ObjectType::EXPLOSION, body()->GetPosition(), 0, {0, 0});
    ShellState::destroy(game);
}


class ExplosionRaycastCallback : public b2RayCastCallback {
    std::vector<BaseObjectState*> _objs;
public:
    float ReportFixture(b2Fixture * fixture, const b2Vec2 & point, const b2Vec2 & normal, float fraction) override {
        _objs.push_back(reinterpret_cast<BaseObjectState*>(fixture->GetBody()->GetUserData().pointer));
        return 1;
    }
    const std::vector<BaseObjectState*>& objs() const {return _objs;}
};

void ExplosionState::prePhysics(Game* game) {
    auto center = body()->GetPosition();
    const int numRays=50;
    const float radius = 15;
    for (int i = 0; i < numRays; i++) {
        float angle =  M_PI * 2 * i / numRays;
        b2Vec2 rayDir(std::cos(angle), std::sin(angle));
        b2Vec2 rayEnd = center + radius * rayDir;

        ExplosionRaycastCallback callback;
        game->world()->RayCast(&callback, center, rayEnd);
        for (auto& obj : callback.objs()) {
            obj->damage(4, DamageType::IMPACT);
        }
    }
    die();
}

class LaserRaycastCallback : public b2RayCastCallback {
    BaseObjectState* _obj = nullptr;
    float _dist = INFINITY;
public:
    float ReportFixture(b2Fixture * fixture, const b2Vec2 & point, const b2Vec2 & normal, float fraction) override {
        if (fraction < _dist) {
            _obj = reinterpret_cast<BaseObjectState*>(fixture->GetBody()->GetUserData().pointer);
            _dist = fraction;
        }
        return _dist;
    }
    BaseObjectState* obj() const {return _obj;}
    float fraction() const {return _dist;}
};

void LaserState::prePhysics(Game* game) {
    _length = _maxLength;
    if (_length) {
        auto center = body()->GetPosition();
        b2Vec2 rayDir(std::cos(body()->GetAngle()), std::sin(body()->GetAngle()));
        b2Vec2 rayEnd = center + _length * rayDir;
        LaserRaycastCallback callback;
        game->world()->RayCast(&callback, center, rayEnd);
        if (callback.obj()) {
            callback.obj()->damage(1, DamageType::THERMAL);
            _length *= callback.fraction();
        }
    }
}

Message LaserState::message() const {
    auto msg = BaseObjectState::message();
    msg["length"] = _length;
    return msg;
}
