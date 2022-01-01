#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <QObject>
#include <QQmlEngine>

namespace constants {
Q_NAMESPACE
QML_NAMED_ELEMENT(Constants)
enum class ObjectType {
    WALL = 0,
    INDESTRUCTABLE_WALL = 1,
    CRATE = 2,
    TANK = 3,
    LAVA = 4,
    STATIC_BOMB = 5,
    TIMED_BOMB = 6,
    ROBO_BOMB = 7,
    CHEST = 8,
    MINE = 9,
    ROCKET = 10,
    MINI_ROCKET = 11,
    SHELL = 12,
    LASER_ROBO = 13,
    LASER_TURRET = 14,
    HEALTH_DROP_ITEM = 15,
    SPEED_DROP_ITEM = 16
};
Q_ENUM_NS(ObjectType)
}  // namespace constants

#endif  // CONSTANTS_HPP