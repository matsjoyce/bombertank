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

#include "playeritem.hpp"
#include <iostream>
#include "bomb.hpp"
#include "walls.hpp"

using namespace std;

enum class PIRenderMessage : unsigned int {
    UPDATE, DESTROY
};

void PlayerItem::attach(std::shared_ptr<Player> pl) {
    player = pl;
}

void PlayerItem::drop() {
    player = {};
}

void BombItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player->render_map()->load_texture("data/images/bomb.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(uses), player->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void BombItem::use() {
    cout << "DROP_BOMB " << uses << endl;
    if (uses) {
        --uses;
        msgpackvar m;
        m["itype"] = as_ui(PIRenderMessage::UPDATE);
        m["uses"] = uses;
        player->item_msg(std::move(m), type());
        auto obj = player->server_map()->add(TimedBomb::TYPE);
        obj->place(player->tcx(), player->tcy());
        obj->destroyed.connect([this]{
            ++uses;
            msgpackvar m;
            m["itype"] = as_ui(PIRenderMessage::UPDATE);
            m["uses"] = uses;
            player->item_msg(std::move(m), type());
        });
    }
}

void BombItem::merge_with(std::shared_ptr<PlayerItem> item) {
    auto i = dynamic_cast<BombItem*>(item.get());
    uses += i->uses;
}

void BombItem::render_handle(msgpackvar && m) {
    switch (static_cast<PIRenderMessage>(m["itype"].as_uint64_t())) {
        case PIRenderMessage::UPDATE: {
            uses = m["uses"].as_uint64_t();
            break;
        }
        default:;
    }
}


void CrateItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player->render_map()->load_texture("data/images/pwall.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(uses), player->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void CrateItem::use() {
    cout << "DROP_WALL " << uses << endl;
    if (uses) {
        --uses;
        msgpackvar m;
        m["itype"] = as_ui(PIRenderMessage::UPDATE);
        m["uses"] = uses;
        player->item_msg(std::move(m), type());
        auto obj = player->server_map()->add(PlacedWall::TYPE);
        obj->place(player->tcx(), player->tcy());
        obj->destroyed.connect([this]{
            ++uses;
            msgpackvar m;
            m["itype"] = as_ui(PIRenderMessage::UPDATE);
            m["uses"] = uses;
            player->item_msg(std::move(m), type());
        });
    }
}

void CrateItem::merge_with(std::shared_ptr<PlayerItem> item) {
    auto i = dynamic_cast<CrateItem*>(item.get());
    uses += i->uses;
}

void CrateItem::render_handle(msgpackvar&& m) {
    switch (static_cast<PIRenderMessage>(m["itype"].as_uint64_t())) {
        case PIRenderMessage::UPDATE: {
            uses = m["uses"].as_uint64_t();
            break;
        }
        default:;
    }
}

map<unsigned int, function<shared_ptr<PlayerItem>()>> load_player_items() {
    decltype(load_player_items()) ret;
    ret[BombItem::TYPE] = make_shared<BombItem>;
    ret[CrateItem::TYPE] = make_shared<CrateItem>;
    return ret;
}
