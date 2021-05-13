#ifndef OBJECTS_OBJECTS_HPP
#define OBJECTS_OBJECTS_HPP

#include "common/Constants.hpp"
#include "Base.hpp"

std::unique_ptr<BaseObjectState> createObjectFromType(ObjectType type);

#endif  // OBJECTS_OBJECTS_HPP