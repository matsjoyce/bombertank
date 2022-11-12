#include "Turret.hpp"

#include <QDebug>
#include <random>

#include "../Game.hpp"
#include "common/Constants.hpp"

TurretState::TurretState() {
}

float TurretState::maxHealth() const {
    return 50;
}

void TurretState::createBodies(b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_staticBody;

    BaseObjectState::createBodies(world, bodyDef);

    b2PolygonShape box;
    // Half-size
    box.SetAsBox(3.0f, 3.0f);

    body()->CreateFixture(&box, 100.0);
    body()->SetFixedRotation(true);
    _targetTurretAngle = body()->GetAngle();
}

class TargettingRaycastCallback : public b2QueryCallback {
    std::set<BaseObjectState*> _objs;
    float _maxDistSq;
    b2Vec2 _center;
public:
    TargettingRaycastCallback(float maxDist, b2Vec2 center) : _maxDistSq(maxDist * maxDist), _center(center) {}
    bool ReportFixture(b2Fixture * fixture) override {
        auto body = fixture->GetBody();
        if ((body->GetPosition() - _center).LengthSquared() <= _maxDistSq) {
            _objs.insert(reinterpret_cast<BaseObjectState*>(body->GetUserData().pointer));
        }
        return true;
    }
    const std::set<BaseObjectState*>& objs() const {return _objs;}
};

void TurretState::prePhysics(Game* game) {
    if (_reload) {
        --_reload;
    }

    b2AABB aabb;
    auto center = body()->GetPosition();
    TargettingRaycastCallback queryCallback(_targettingRange, center);
    aabb.lowerBound = center - b2Vec2{_targettingRange, _targettingRange};
    aabb.upperBound = center + b2Vec2{_targettingRange, _targettingRange};
    game->world()->QueryAABB(&queryCallback, aabb);
    bool hasTarget = false;
    for (auto& obj : queryCallback.objs()) {
        if (obj->side() != side()) {
            auto targetVector = (obj->body()->GetPosition() - center);
            _targetTurretAngle = std::atan2(targetVector.y, targetVector.x);
            hasTarget = true;
            break;
        }
    }

    auto angleDiff = std::fmod(_targetTurretAngle - _turretAngle, 2.0f * M_PIf);
    if (angleDiff > M_PI) {
        angleDiff -= 2 * M_PI;
    }
    _turretAngle += std::min(_slewRate, std::max(-_slewRate, angleDiff));

    if (hasTarget && !_reload && std::abs(angleDiff) < 0.1) {
        auto forward = b2Vec2{std::cos(_turretAngle), std::sin(_turretAngle)};
        auto sideways = b2Vec2{forward.y, forward.x};

        const auto speed = 100;
        const auto maxSidewaysVelocity = speed / 20;
        std::uniform_real_distribution<float> distribution(-maxSidewaysVelocity, maxSidewaysVelocity);
        auto velocity = speed * forward + distribution(game->randomGenerator()) * sideways;
        game->addObject(constants::ObjectType::MG_SHELL, center + 5 * forward,
                                        std::atan2(velocity.y, velocity.x), velocity);
        _reload = 2;
        return;
    }
}

Message TurretState::message() const {
    auto msg = BaseObjectState::message();
    msg["turretAngle"] = _turretAngle;
    return msg;
}
