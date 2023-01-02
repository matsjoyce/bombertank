#ifndef OBJECTS_MARKERS_HPP
#define OBJECTS_MARKERS_HPP

#include "Base.hpp"

class StartZoneState : public BaseObjectState {
   public:
    using BaseObjectState::BaseObjectState;
    float maxHealth() const override { return 1.0f; }
};
REGISTER_STATE(StartZoneState)


#endif  // OBJECTS_MARKERS_HPP
