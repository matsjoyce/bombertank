/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "attackutils.hpp"
#include "../map.hpp"
#include <iostream>

using namespace std;


unsigned int progressive_kill_in_direction(ServerMap* sm, int x, int y, unsigned int size, unsigned int length, Orientation::Orientation direction, int damage, DamageType type) {
    for (auto& obj : sm->collides_by_moving(x, y, size, size, direction, length, false)) {
        damage = obj.second->take_damage(damage, type);
        if (obj.second->alive()) {
            return max(0, obj.second->separation_distance(x, y, 0, 0));
            break;
        }
    }
    return length;
}
