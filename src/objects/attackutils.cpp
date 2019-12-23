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
#include "../servermap.hpp"
#include <iostream>

using namespace std;


unsigned int progressive_kill_in_direction(ServerMap* sm, Point start, unsigned int size, unsigned int length,
                                           Orientation::Orientation direction, int damage, DamageType type, unsigned int layer) {
    auto r = Rect(Size(size, length).rotate(direction));
    r.set_dir_center(opposite(direction), start);
    auto dist_r = Rect(start, start);
    for (auto& obj : sm->collides(r, [&dist_r](objptr obj){return obj->separation_distance(dist_r);}, layer)) {
        damage = obj.second->take_damage(damage, type);
        if (obj.second->alive()) {
            return max(0, obj.first);
            break;
        }
    }
    return length;
}
