#include "Walls.hpp"
#include "../Game.hpp"

#include <QDebug>

template<class> inline constexpr bool always_false_v = false;

void AbstractWallState::createBodies(Game* game, b2World& world, b2BodyDef& bodyDef) {
    bodyDef.type = b2_staticBody;

    BaseObjectState::createBodies(game, world, bodyDef);

    auto& data = game->dataForType(type());

    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            b2PolygonShape box;
            // Half-size
            box.SetAsBox(4.5f, 4.5f);

            body()->CreateFixture(&box, 1.0);
        }
        else if constexpr (std::is_same_v<T, BoxGeometry>) {
            b2PolygonShape box;
            box.SetAsBox(arg.width / 2, arg.height / 2);

            body()->CreateFixture(&box, 1.0);
        }
        else if constexpr (std::is_same_v<T, PolyGeometry>) {
            b2PolygonShape box;
            std::vector<b2Vec2> points;
            for (auto& point : arg.points) {
                points.emplace_back(b2Vec2{point.first, point.second});
            }
            box.Set(points.data(), points.size());

            body()->CreateFixture(&box, 1.0);
        }
        else {
            static_assert(always_false_v<T>, "non-exhaustive visitor!");
        }
    }, data.geometry);
}

float WallState::maxHealth() const { return 100; }

float IndestructableWallState::maxHealth() const { return 1; }
