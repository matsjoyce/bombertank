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

float MineDetector::maxReload() {
    return 20;
}

Message MineDetector::message() {
    auto msg = TankModule::message();
    std::vector<msgpack::type::variant> encodedPoints;
    for (auto point: _points) {
        encodedPoints.push_back(std::vector<msgpack::type::variant>{point.x, point.y});
    }
    msg["points"] = encodedPoints;
    return msg;
}

void MineDetector::reloaded() {
    _points.clear();
}
