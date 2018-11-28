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

#include "walls.hpp"

using namespace std;

void Wall::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/wall.png"));
    position_sprite(sp);
    rt.draw(sp);
}

unsigned int Wall::max_hp() {
    return 100;
}

unsigned int Wall::render_layer() {
    return 2;
}

void IndestructableWall::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/iwall.png"));
    position_sprite(sp);
    rt.draw(sp);
}

unsigned int IndestructableWall::take_damage(unsigned int /*damage*/, DamageType /*dt*/) {
    // You have no power here!
    return 0;
}

unsigned int IndestructableWall::render_layer() {
    return 2;
}

void PlacedWall::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/pwall.png"));
    position_sprite(sp);
    rt.draw(sp);
}

unsigned int PlacedWall::max_hp() {
    return 50;
}

unsigned int PlacedWall::render_layer() {
    return 2;
}
