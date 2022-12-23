#ifndef OBJECTS_ACTIONS_MINE_DETECTOR_HPP
#define OBJECTS_ACTIONS_MINE_DETECTOR_HPP

#include "TankModule.hpp"
#include "box2d/box2d.h"

class MineDetector : public TankModule {
    std::vector<b2Vec2> _points;
public:
    int type() override { return 15; }
    void act(Game* game, TankState* tank) override;
    float maxReload() override;
    Message message() override;
    void reloaded() override;
};

#endif // OBJECTS_ACTIONS_MINE_DETECTOR_HPP

