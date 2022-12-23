#ifndef OBJECTS_UTILS_HPP
#define OBJECTS_UTILS_HPP

#include "Base.hpp"

std::vector<BaseObjectState*> getPrioritsedTargets(const std::set<BaseObjectState*>& objs, int side, b2Vec2 center);

#endif  // OBJECTS_UTILS_HPP
