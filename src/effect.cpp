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

#include "effect.hpp"
#include "rendermap.hpp"

using namespace std;

Effect::Effect(RenderMap* map_, unsigned int id_, Point pos_, Orientation::Orientation ori) : id(id_), pos(pos_), orientation(ori), map(map_) {
}

void Effect::update() {
}

void Effect::render(sf::RenderTarget& /*rt*/) {
}

void Effect::destroy() {
    map->remove_effect(id);
}

unsigned int Effect::layer() {
    return 0;
}
