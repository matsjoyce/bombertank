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

#include "projectiles.hpp"

using namespace std;

Rocket::Rocket(unsigned int id_, Map* map_) : StaticBomb(id_, map_) {
    set_size(2, 4);
}

void Rocket::update() {
    accelerate(1);
    --time_left;
    if (!time_left) {
        destroy();
    }
}

void Rocket::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/rocket.png"));
    position_sprite(sp);
    sp.setOrigin(2, 2);
    rt.draw(sp);
}

unsigned int Rocket::render_layer() {
    return 4;
}

void Rocket::collision(objptr obj, bool caused_by_self) {
    if (obj->side() != side()) {
        _generate_move();
        destroy();
    }
}
