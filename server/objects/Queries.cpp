#include "Queries.hpp"
#include "../Game.hpp"


class CircleCallback : public b2QueryCallback {
    std::set<BaseObjectState*> _objs;
    float _maxDistSq;
    b2Vec2 _center;
public:
    CircleCallback(float maxDist, b2Vec2 center) : _maxDistSq(maxDist * maxDist), _center(center) {}
    bool ReportFixture(b2Fixture * fixture) override {
        auto body = fixture->GetBody();
        if ((body->GetPosition() - _center).LengthSquared() <= _maxDistSq) {
            _objs.insert(reinterpret_cast<BaseObjectState*>(body->GetUserData().pointer));
        }
        return true;
    }
    const std::set<BaseObjectState*>& objs() const {return _objs;}
};

std::set<BaseObjectState*> queryObjectsInCircle(Game* game, b2Vec2 center, float radius) {
    b2AABB aabb;
    CircleCallback queryCallback(radius, center);
    aabb.lowerBound = center - b2Vec2{radius, radius};
    aabb.upperBound = center + b2Vec2{radius, radius};
    game->world()->QueryAABB(&queryCallback, aabb);
    return queryCallback.objs();
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

std::set<BaseObjectState*> queryObjectsInIsoscelesTriangle(Game* game, b2Vec2 topPoint, b2Vec2 direction, float distance, float halfAngle) {
    b2Rot rotation(halfAngle);
    b2Vec2 v1 = topPoint;
    b2Vec2 v2 = v1 + b2Mul(rotation, distance * direction);
    b2Vec2 v3 = v1 + b2MulT(rotation, distance * direction);
    b2AABB aabb;
    aabb.lowerBound = b2Min(v1, b2Min(v2, v3));
    aabb.upperBound = b2Max(v1, b2Max(v2, v3));
    TriangleRaycastCallback queryCallback(v1, v2, v3);
    game->world()->QueryAABB(&queryCallback, aabb);
    return queryCallback.objs();
}

class AllObjectsRaycastCallback : public b2RayCastCallback {
    std::set<BaseObjectState*> _objs;
public:
    float ReportFixture(b2Fixture * fixture, const b2Vec2 & point, const b2Vec2 & normal, float fraction) override {
        _objs.insert(reinterpret_cast<BaseObjectState*>(fixture->GetBody()->GetUserData().pointer));
        return 1;
    }
    const std::set<BaseObjectState*>& objs() const {return _objs;}
};

std::set<BaseObjectState*> raycastAllObjects(Game* game, b2Vec2 start, b2Vec2 end) {
    AllObjectsRaycastCallback callback;
    game->world()->RayCast(&callback, start, end);
    return callback.objs();
}

class FirstObjectRaycastCallback : public b2RayCastCallback {
    BaseObjectState* _obj = nullptr;
    float _dist = INFINITY;
    std::function<bool(BaseObjectState*)> _pred;
public:
    FirstObjectRaycastCallback(std::function<bool(BaseObjectState*)> pred) : _pred(pred) {}
    float ReportFixture(b2Fixture * fixture, const b2Vec2 & point, const b2Vec2 & normal, float fraction) override {
        auto obj = reinterpret_cast<BaseObjectState*>(fixture->GetBody()->GetUserData().pointer);
        if (fraction < _dist && _pred(obj)) {
            _obj = obj;
            _dist = fraction;
        }
        return _dist;
    }
    BaseObjectState* obj() const {return _obj;}
    float fraction() const {return _dist;}
};

std::optional<std::tuple<BaseObjectState*, float>> raycastNearestObject(Game* game, b2Vec2 start, b2Vec2 end, std::function<bool(BaseObjectState*)> pred) {
    FirstObjectRaycastCallback callback(pred);
    game->world()->RayCast(&callback, start, end);
    if (callback.obj() != nullptr) {
        return {{callback.obj(), callback.fraction()}};
    }
    return {};
}

bool hasCollisionCategory(BaseObjectState* obj, int category) {
    return hasCollisionCategory(obj->body(), category);
}

bool hasCollisionCategory(b2Body* obj, int category) {
    for (b2Fixture* f = obj->GetFixtureList(); f; f = f->GetNext()) {
        if (f->GetFilterData().maskBits & category) {
            return true;
        }
    }
    return false;
}
