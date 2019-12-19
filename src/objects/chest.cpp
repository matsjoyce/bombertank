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
#include "effects.hpp"
#include <iterator>

using namespace std;

Chest::Chest(unsigned int id_, Map* map_) : Object(id_, map_) {
    if (auto sm = server_map()) {
        destroyed.connect([this, sm]{
            auto pos = nw_corner();
            sm->on_frame(sm->frame() + sm->frame_rate * 30, [pos, sm]{
                auto obj = sm->add(Chest::TYPE);
                obj->set_nw_corner(pos);
                obj->_generate_move();
            });
        });
    }
}

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
        vector<unsigned int> options;
        options.push_back(-1);
        for (auto& item : pl->items()) {
            if (dynamic_pointer_cast<UsesPlayerItem>(item.second)) {
                options.push_back(item.first);
            }
        }

        if (options.size()) {
            uniform_int_distribution<int> distribution(0, options.size() - 1);
            auto item_id = options[distribution(map->random_generator())];
            if (item_id == static_cast<unsigned int>(-1)) {
                pl->heal(50);

                msgpackvar m;
                m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
                m["type"] = as_ui(RenderObjectMessage::END);
                m["id"] = id;
                m["item"] = "+50 hp";
                m["side"] = pl->side();
                server_map()->event(shared_from_this(), std::move(m));
            }
            else {
                auto item = load_player_items()[item_id]();
                pl->add_item(item);

                msgpackvar m;
                m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
                m["type"] = as_ui(RenderObjectMessage::END);
                m["id"] = id;
                m["item"] = item->name();
                m["side"] = pl->side();
                server_map()->event(shared_from_this(), std::move(m));
            }
        }
        destroy();
    }
}

void Chest::render_handle(msgpackvar m) {
    switch (static_cast<RenderObjectMessage>(m["type"].as_uint64_t())) {
        case RenderObjectMessage::END: {
            render_map()->add_effect<PopupText>(m["side"].as_uint64_t(), m["item"].as_string());
            break;
        }
        default: Object::render_handle(std::move(m));
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
