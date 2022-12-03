#include "Projectiles.hpp"
#include "../Game.hpp"

const float IMPULSE_TO_DAMAGE = 300.0f;

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

// https://stackoverflow.com/a/2049593
float sign(b2Vec2 p1, b2Vec2 p2, b2Vec2 p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool pointInTriangle(b2Vec2 pt, b2Vec2 v1, b2Vec2 v2, b2Vec2 v3) {
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

class TriangleRaycastCallback : public b2QueryCallback {
    std::set<BaseObjectState*> _objs;
    b2Vec2 _v1, _v2, _v3;
public:
    TriangleRaycastCallback(b2Vec2 v1, b2Vec2 v2, b2Vec2 v3) : _v1(v1), _v2(v2), _v3(v3) {}
    bool ReportFixture(b2Fixture * fixture) override {
        auto body = fixture->GetBody();
        if (pointInTriangle(body->GetPosition(), _v1, _v2, _v3)) {
            _objs.insert(reinterpret_cast<BaseObjectState*>(body->GetUserData().pointer));
        }
        return true;
    }
    const std::set<BaseObjectState*>& objs() const {return _objs;}
};

void HomingRocketState::prePhysics(Game* game) {
    RocketState::prePhysics(game);
    // Weathercock
    auto velocity = body()->GetLinearVelocityFromLocalPoint({0, 0});
    body()->SetTransform(body()->GetPosition(), std::atan2(velocity.y, velocity.x));

    // Build an isosceles triangle which forms the "search cone" using our position
    b2Rot rotation(M_PI/8);
    auto forward = body()->GetWorldVector({1, 0});
    auto dist = 50;
    b2Vec2 v1 = body()->GetPosition();
    b2Vec2 v2 = v1 + b2Mul(rotation, dist * forward);
    b2Vec2 v3 = v1 + b2MulT(rotation, dist * forward);
    b2AABB aabb;
    aabb.lowerBound = b2Min(v1, b2Min(v2, v3));
    aabb.upperBound = b2Max(v1, b2Max(v2, v3));
    TriangleRaycastCallback queryCallback(v1, v2, v3);
    game->world()->QueryAABB(&queryCallback, aabb);
    std::vector<BaseObjectState*> objs(queryCallback.objs().begin(), queryCallback.objs().end());
    std::sort(objs.begin(), objs.end(), [v1](BaseObjectState* left, BaseObjectState* right) {
        // Lexicographical sort of (hostility, distance)
        if (left->hostility() == right->hostility()) {
            return (left->body()->GetPosition() - v1).LengthSquared() < (right->body()->GetPosition() - v1).LengthSquared();
        }
        return left->hostility() > right->hostility();
    });
    for (auto& obj : queryCallback.objs()) {
        if (obj->side() != side() && obj->hostility() > Hostility::NON_HOSTILE) {
            auto targetVector = (obj->body()->GetPosition() - v1);
            bool goLeft = b2Cross(targetVector, forward) > 0;
            body()->ApplyLinearImpulseToCenter(body()->GetMass() * 8 * (goLeft ? -1 : 1) * body()->GetWorldVector({0, 1}), true);
            break;
        }
    }
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
