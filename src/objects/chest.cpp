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

#include "chest.hpp"
#include "player.hpp"
#include "playeritem.hpp"
#include <iterator>

using namespace std;

void Chest::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/chest.png"));
    position_sprite(sp);
    rt.draw(sp);
}

unsigned int Chest::render_layer() {
    return 2;
}

void Chest::collision(objptr obj, bool /*caused_by_self*/) {
    if (auto pl = dynamic_pointer_cast<Player>(obj)) {
        auto pis = load_player_items();
        if (!pis.size()) return;
        uniform_int_distribution<int> distribution(0, pis.size() - 1);
        auto iter = pis.begin();
        advance(iter, distribution(pl->server_map()->random_generator()));
        pl->add_item(iter->second());
        destroy();
    }
}

void LevelUp::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/level_up.png"));
    position_sprite(sp);
    rt.draw(sp);
}

unsigned int LevelUp::render_layer() {
    return 2;
}

void LevelUp::update() {
    for (auto& obj : server_map()->collides(*this)) {
        if (auto pl = dynamic_pointer_cast<Player>(obj.second)) {
            pl->level_up();
            destroy();
        }
    }
}

unsigned int LevelUp::layer() {
    return 4;
}
