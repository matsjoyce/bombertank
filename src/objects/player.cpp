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

#include "player.hpp"
#include "bomb.hpp"
#include "walls.hpp"
#include <iostream>
#include "playeritem.hpp"

using namespace std;

enum class PlayerServerMessage : unsigned int {
    START_MOVE = static_cast<unsigned int>(RenderObjectMessage::END),
    END_MOVE, START_PRIMARY, START_SECONDARY, END_PRIMARY, END_SECONDARY, SECONDARY_L, SECONDARY_R, PRIMARY_SET, TRANSFER, USE_ITEM
};

enum class PlayerRenderMessage : unsigned int {
    TRANSFER = static_cast<unsigned int>(RenderObjectMessage::END),
    ADD_ITEM, FOR_ITEM, SET_PRIMARY, SET_SECONDARY
};

struct PlayerSettings {
    sf::Keyboard::Key up, down, left, right;
    sf::Keyboard::Key primary, secondary;
    sf::Keyboard::Key sec_l, sec_r, pri_set;
    sf::Color color;
};

map<int, PlayerSettings> player_settings = {
    {0, {
            sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D,
            sf::Keyboard::B, sf::Keyboard::G,
            sf::Keyboard::Z, sf::Keyboard::C, sf::Keyboard::X,
            sf::Color(128, 0, 0)
    }},
    {1, {
            sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right,
            sf::Keyboard::Enter, sf::Keyboard::RBracket,
            sf::Keyboard::Comma, sf::Keyboard::Slash, sf::Keyboard::Period,
            sf::Color(0, 128, 0)
    }},
};

Player::Player(unsigned int id_, Map* map_) : Object(id_, map_) {
    if (render_map()) {
        destroyed.connect([this] {
            render_map()->add_effect<DeadPlayer>(center(), orientation());
        });
        setup_keys();
        side_changed.connect([this] {
            setup_keys();
        });
    }
    auto fid = map->paused.connect([this] {
        direction_stack.clear();
        if (items.count(primary_item)) {
            items[primary_item]->try_end();
        }
        if (items.count(secondary_item)) {
            items[secondary_item]->try_end();
        }
    });
    destroyed.connect([this, fid] {
        map->paused.disconnect(fid);
    });
}

void Player::render(sf::RenderTarget& rt) {
    if (speed()) {
        auto t = fmod(anim_clock.getElapsedTime().asSeconds() * 24, 4);
        tex_name = t < 1 ? "data/images/tank1.png" : t < 2 ? "data/images/tank2.png" : t < 3 ? "data/images/tank3.png" : "data/images/tank4.png";
    }
    sf::Sprite sp(render_map()->load_texture(tex_name));
    position_sprite(sp);
    rt.draw(sp);

    sf::Sprite sp2(render_map()->load_texture("data/images/tank_colored.png"));
    position_sprite(sp2);
    sp2.setColor(player_settings[side()].color);
    rt.draw(sp2);
}

void Player::handle_keypress(sf::Keyboard::Key key, bool is_down) {
    msgpackvar m;
    m["mtype"] = as_ui(ToServerMessage::FOROBJ);
    m["id"] = id;
    auto& settings = player_settings[side()];
    if (key == settings.primary) {
        m["type"] = as_ui(is_down ? PlayerServerMessage::START_PRIMARY : PlayerServerMessage::END_PRIMARY);
    }
    else if (key == settings.secondary) {
        m["type"] = as_ui(is_down ? PlayerServerMessage::START_SECONDARY : PlayerServerMessage::END_SECONDARY);
    }
    else if (key == settings.sec_l) {
        if (!is_down) return;
        m["type"] = as_ui(PlayerServerMessage::SECONDARY_L);
    }
    else if (key == settings.sec_r) {
        if (!is_down) return;
        m["type"] = as_ui(PlayerServerMessage::SECONDARY_R);
    }
    else if (key == settings.pri_set) {
        if (!is_down) return;
        m["type"] = as_ui(PlayerServerMessage::PRIMARY_SET);
    }
    else {
        m["type"] = is_down ? as_ui(PlayerServerMessage::START_MOVE) : as_ui(PlayerServerMessage::END_MOVE);
        m["ori"] = key == settings.left ? as_ui(Orientation::W) :
                   key == settings.right ? as_ui(Orientation::E) :
                   key == settings.up ? as_ui(Orientation::N) :
                   key == settings.down ? as_ui(Orientation::S) : as_ui(-1);
    }
    render_map()->event(std::move(m));
}

void Player::handle(msgpackvar m) {
    switch (static_cast<PlayerServerMessage>(m["type"].as_uint64_t())) {
        case PlayerServerMessage::START_MOVE: {
            direction_stack.push_back(static_cast<Orientation::Orientation>(m["ori"].as_uint64_t()));
            break;
        }
        case PlayerServerMessage::END_MOVE: {
            auto pos = find(direction_stack.begin(), direction_stack.end(), static_cast<Orientation::Orientation>(m["ori"].as_uint64_t()));
            if (pos == direction_stack.end()) {
                cout << "No move to end " << m["ori"].as_uint64_t() << endl;
            }
            else {
                direction_stack.erase(pos);
            }
            break;
        }
        case PlayerServerMessage::START_PRIMARY: {
            if (items.count(primary_item)) {
                items[primary_item]->try_start();
            }
            break;
        }
        case PlayerServerMessage::END_PRIMARY: {
            if (items.count(primary_item)) {
                items[primary_item]->try_end();
            }
            break;
        }
        case PlayerServerMessage::START_SECONDARY: {
            if (primary_item != secondary_item && items.count(secondary_item)) {
                items[secondary_item]->try_start();
            }
            break;
        }
        case PlayerServerMessage::END_SECONDARY: {
            if (primary_item != secondary_item && items.count(secondary_item)) {
                items[secondary_item]->try_end();
            }
            break;
        }
        case PlayerServerMessage::SECONDARY_L: {
            auto prev = lower_bound(items.begin(), items.end(), secondary_item, [](auto& a, auto& b){return a.first < b;});
            unsigned int val = -1;
            if (prev == items.end()) {
                if (items.size()) {
                    val = (--items.end())->first;
                }
            }
            else if (prev->first == secondary_item) {
                if (prev == items.begin()) {
                    val = (--items.end())->first;
                }
                else {
                    val = (--prev)->first;
                }
            }
            else {
                val = prev->first;
            }
            set_secondary(val);
            break;
        }
        case PlayerServerMessage::SECONDARY_R: {
            auto next = upper_bound(items.begin(), items.end(), secondary_item, [](auto& a, auto& b){return a < b.first;});
            unsigned int val = -1;
            if (next == items.end()) {
                if (items.size()) {
                    val = items.begin()->first;
                }
            }
            else {
                val = next->first;
            }
            set_secondary(val);
            break;
        }
        case PlayerServerMessage::PRIMARY_SET: {
            set_primary(secondary_item);
            break;
        }
        default: Object::handle(m);
    }
}

void Player::render_handle(msgpackvar m) {
    switch (static_cast<PlayerRenderMessage>(m["type"].as_uint64_t())) {
        case PlayerRenderMessage::TRANSFER: {
            lives_ = m["lives"].as_uint64_t();
            primary_item = m["primary"].as_uint64_t();
            secondary_item = m["secondary"].as_uint64_t();
            break;
        }
        case PlayerRenderMessage::ADD_ITEM: {
            auto type = m["item"].as_uint64_t();
            auto item = items[type] = load_player_items()[type]();
            item->attach(dynamic_pointer_cast<Player>(shared_from_this()));
            break;
        }
        case PlayerRenderMessage::FOR_ITEM: {
            auto type = m["item"].as_uint64_t();
            if (items.count(type)) {
                items[type]->render_handle(std::move(m));
            }
            else {
                cout << "Event for non-existent item " << type << endl;
            }
            break;
        }
        case PlayerRenderMessage::SET_PRIMARY: {
            primary_item = m["primary"].as_uint64_t();
            break;
        }
        case PlayerRenderMessage::SET_SECONDARY: {
            secondary_item = m["secondary"].as_uint64_t();
            break;
        }
        default: Object::render_handle(m);
    }
}


void Player::setup_keys() {
    if (side() != static_cast<unsigned int>(-1)) {
        if (auto rmap = render_map()) {
            if (rmap->side() == side()) {
                rmap->follow(shared_from_this());
                rmap->register_keypress(player_settings[side()].left, id);
                rmap->register_keypress(player_settings[side()].right, id);
                rmap->register_keypress(player_settings[side()].up, id);
                rmap->register_keypress(player_settings[side()].down, id);
                rmap->register_keypress(player_settings[side()].primary, id);
                rmap->register_keypress(player_settings[side()].secondary, id);
                rmap->register_keypress(player_settings[side()].sec_l, id);
                rmap->register_keypress(player_settings[side()].sec_r, id);
                rmap->register_keypress(player_settings[side()].pri_set, id);
            }
        }
    }
}

void Player::post_constructor() {
    Object::post_constructor();
    if (auto sm = server_map()) {
        add_item(make_shared<BombItem>());
        add_item(make_shared<CrateItem>());
        add_item(make_shared<MineItem>());
        add_item(make_shared<LaserItem>());
        add_item(make_shared<ChargeItem>());
        set_primary(BombItem::TYPE);
        set_secondary(CrateItem::TYPE);
    }
}

void Player::update() {
    for (auto& item : items) {
        item.second->update();
    }
    if (direction_stack.size()) {
        if (direction() != direction_stack.back()) {
            set_direction(direction_stack.back());
            set_orientation(direction_stack.back());
        }
        accelerate(max_speed - speed());
    }
}

unsigned int Player::max_hp() {
    return 150;
}

unsigned int Player::render_layer() {
    return 6;
}

void Player::transfer(objptr obj) {
    auto pl = dynamic_cast<Player*>(obj.get());
    pl->lives_ = lives_ - 1;
    pl->set_side(side());
    pl->primary_item = primary_item;
    pl->secondary_item = secondary_item;
    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
    m["type"] = as_ui(PlayerRenderMessage::TRANSFER);
    m["id"] = pl->id;
    m["lives"] = pl->lives_;
    m["primary"] = pl->primary_item;
    m["secondary"] = pl->secondary_item;
    server_map()->event(obj, std::move(m));
}

void Player::render_hud(sf::RenderTarget& rt) {
    Object::render_hud(rt);
    auto rm = render_map();

    auto texbg = rm->load_texture("data/images/life_used.png");
    texbg.setRepeated(true);
    sf::Sprite sp_bg(texbg);
    sp_bg.setTextureRect(sf::IntRect(0, 0, 30, 10));
    sp_bg.setPosition(204, 0);
    rt.draw(sp_bg);

    auto texfg = rm->load_texture("data/images/life.png");
    texfg.setRepeated(true);
    sf::Sprite sp_fg(texfg);
    sp_fg.setTextureRect(sf::IntRect(0, 0, 10 * lives_, 10));
    sp_fg.setPosition(204, 0);
    rt.draw(sp_fg);

    sf::RectangleShape lowerbg(sf::Vector2f(rt.getView().getSize().x, STANDARD_OBJECT_SIZE + 6));
    lowerbg.setFillColor(sf::Color(0, 0, 0, 128));
    lowerbg.setPosition(0, rt.getView().getSize().y - lowerbg.getSize().y);
    rt.draw(lowerbg);

    int x = 3;

    for (auto& item : items) {
        auto pos = sf::Vector2f(x, lowerbg.getPosition().y + 3);
        if (item.first == primary_item) {
            sf::RectangleShape bg(sf::Vector2f(STANDARD_OBJECT_SIZE + 4, STANDARD_OBJECT_SIZE + 4));
            bg.setPosition(pos - sf::Vector2f(2, 2));
            bg.setFillColor(sf::Color(255, 0, 0, item.second->active() ? 255 : 128));
            rt.draw(bg);
        }
        else if (item.first == secondary_item) {
            sf::RectangleShape bg(sf::Vector2f(STANDARD_OBJECT_SIZE + 4, STANDARD_OBJECT_SIZE + 4));
            bg.setPosition(pos - sf::Vector2f(2, 2));
            bg.setFillColor(sf::Color(0, 255, 0, item.second->active() ? 255 : 128));
            rt.draw(bg);
        }
        item.second->render(rt, pos);
        x += STANDARD_OBJECT_SIZE + 6;
    }
}

void Player::add_item(shared_ptr<PlayerItem> item) {
    if (auto sm = server_map()) {
        if (items.count(item->type())) {
            items[item->type()]->merge_with(item);
        }
        else {
            item->attach(dynamic_pointer_cast<Player>(shared_from_this()));
            items.emplace(item->type(), item);
            msgpackvar m;
            m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
            m["id"] = id;
            m["type"] = as_ui(PlayerRenderMessage::ADD_ITEM);
            m["item"] = item->type();
            sm->event(shared_from_this(), std::move(m));
        }
    }
}

void Player::item_msg(msgpackvar&& m, unsigned int type) {
    m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
    m["id"] = id;
    m["type"] = as_ui(PlayerRenderMessage::FOR_ITEM);
    m["item"] = type;
    server_map()->event(shared_from_this(), std::move(m));
}

void Player::set_primary(unsigned int pri) {
    primary_item = pri;
    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
    m["id"] = id;
    m["type"] = as_ui(PlayerRenderMessage::SET_PRIMARY);
    m["primary"] = pri;
    server_map()->event(shared_from_this(), std::move(m));
}

void Player::set_secondary(unsigned int sec) {
    secondary_item = sec;
    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
    m["id"] = id;
    m["type"] = as_ui(PlayerRenderMessage::SET_SECONDARY);
    m["secondary"] = sec;
    server_map()->event(shared_from_this(), std::move(m));
}

void DeadPlayer::render(sf::RenderTarget& rt) {
    sf::Sprite sp(map->load_texture("data/images/tank_dead.png"));
    sp.setOrigin(sf::Vector2f(sp.getTextureRect().width / 2, sp.getTextureRect().height / 2));
    sp.setPosition(pos);
    sp.setRotation(angle(orientation));
    rt.draw(sp);
}

unsigned int DeadPlayer::layer() {
    return 1;
}
