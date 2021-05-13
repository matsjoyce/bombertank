#ifndef OBJECTS_OBJECTS_HPP
#define OBJECTS_OBJECTS_HPP

#include "Base.hpp"

std::unique_ptr<BaseObjectState> createObjectFromType(int type);

#endif  // OBJECTS_OBJECTS_HPP