#ifndef OBJECTS_OBJECTS_HPP
#define OBJECTS_OBJECTS_HPP

#include "Base.hpp"
#include "common/Constants.hpp"

std::unique_ptr<BaseObjectState> createObjectFromType(constants::ObjectType type);

#endif  // OBJECTS_OBJECTS_HPP