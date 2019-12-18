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

#include "floors.hpp"

using namespace std;

unsigned int Lava::layer() {
    return 0;
}

unsigned int Lava::render_layer() {
    return 0;
}

void Lava::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/lava.png"));
    position_sprite(sp);
    rt.draw(sp);
}

unsigned int Lava::take_damage(unsigned int /*damage*/, DamageType /*dt*/) {
    // You have no power here!
    return 0;
}

void Lava::update() {
    for (auto& obj : server_map()->collides(*this)) {
        if (obj.second->id != id && !obj.second->is_projectile()) {
            obj.second->take_damage(2, DamageType::HEAT);
        }
    }
}
