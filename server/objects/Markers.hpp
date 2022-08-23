#ifndef OBJECTS_MARKERS_HPP
#define OBJECTS_MARKERS_HPP

#include "Base.hpp"

class StartZoneState : public BaseObjectState {
   public:
    using BaseObjectState::BaseObjectState;
    constants::ObjectType type() const override { return constants::ObjectType::START_ZONE; }
    float maxHealth() const override { return 1.0f; }
};


#endif  // OBJECTS_MARKERS_HPP
