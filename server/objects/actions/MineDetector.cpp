#include "MineDetector.hpp"
#include "../Tank.hpp"
#include "../../Game.hpp"
#include "../Queries.hpp"

void MineDetector::act(Game* game, TankState* tank) {
    TankModule::act(game, tank);
    for (auto obj : queryObjectsInCircle(game, tank->body()->GetPosition(), 25)) {
        if (obj->type() == static_cast<int>(constants::ObjectType::MINE)) {
            _points.push_back(obj->body()->GetPosition());
        }
    }
}

float MineDetector::maxReload() const {
    return 20;
}

void MineDetector::fillMessage(bt_messages::ToClientMessage_TankModuleUpdates& msg) const {
    TankModule::fillMessage(msg);
    for (auto point: _points) {
        auto p = msg.add_points();
        p->set_x(point.x);
        p->set_y(point.y);
    }
}

void MineDetector::reloaded() {
    _points.clear();
}
