#ifndef OBJECTS_QUERIES_HPP
#define OBJECTS_QUERIES_HPP

#include "Base.hpp"
#include <tuple>
#include <functional>

std::set<BaseObjectState*> queryObjectsInCircle(Game* game, b2Vec2 center, float radius);
std::set<BaseObjectState*> queryObjectsInIsoscelesTriangle(Game* game, b2Vec2 topPoint, b2Vec2 direction, float distance, float halfAngle);

std::set<BaseObjectState*> raycastAllObjects(Game* game, b2Vec2 start, b2Vec2 end);
std::optional<std::tuple<BaseObjectState*, float>> raycastNearestObject(Game* game, b2Vec2 start, b2Vec2 end, std::function<bool(BaseObjectState*)> pred);

bool hasCollisionCategory(BaseObjectState* obj, int category);
bool hasCollisionCategory(b2Body* obj, int category);

#endif  // OBJECTS_QUERIES_HPP
