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
    START_ZONE = 3,
    LAVA = 4,
    BOMB = 5,
    TIMED_BOMB = 6,
    ROBO_BOMB = 7,
    CHEST = 8,
    MINE = 9,
    ROCKET = 10,
    MINI_ROCKET = 11,
    SHELL = 12,
    LASER_ROBO = 13,
    MG_TURRET = 14,
    HEALTH_DROP_ITEM = 15,
    SPEED_DROP_ITEM = 16,
    MG_SHELL = 17,
    TANK = 18,
    EXPLOSION = 19,
    LASER_TURRET = 20,
    LASER = 21,
    HOMING_ROCKET = 22,
};
Q_ENUM_NS(ObjectType)

enum StatusType {
    STUNNED = 1,
    INVISIBLE = 2,
};
Q_DECLARE_FLAGS(StatusTypes, StatusType)
Q_DECLARE_OPERATORS_FOR_FLAGS(StatusTypes)
Q_FLAG_NS(StatusTypes)
}  // namespace constants

#endif  // CONSTANTS_HPP
