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

#include "loader.hpp"

#include "walls.hpp"
#include "player.hpp"
#include "bomb.hpp"
#include "chest.hpp"
#include "projectiles.hpp"
#include "floors.hpp"

using namespace std;


template<class T> objptr creator(unsigned int id, Map* m) {
    auto obj = make_shared<T>(id, m);
    obj->post_constructor();
    return obj;
}

bool loaded;
map<unsigned int, function<objptr(unsigned int, Map*)>> object_creators;

template<class T> void load() {
    if (object_creators.count(T::TYPE)) {
        throw runtime_error("Duplicate ids");
    }
    object_creators[T::TYPE] = &creator<T>;
}

map<unsigned int, function<objptr(unsigned int, Map*)>> load_objects() {
    if (!loaded) {
        load<Wall>();
        load<IndestructableWall>();
        load<PlacedWall>();
        load<Player>();
        load<StaticBomb>();
        load<TimedBomb>();
        load<RoboBomb>();
        load<Mine>();
        load<Chest>();
        load<LevelUp>();
        load<Rocket>();
        load<MiniRocket>();
        load<Lava>();
        load<LaserRobo>();
        loaded = true;
    }
    return object_creators;
}
