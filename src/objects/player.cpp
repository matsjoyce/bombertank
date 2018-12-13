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
#include "effects.hpp"
#include <iostream>
#include "playeritem.hpp"

using namespace std;

enum class PlayerServerMessage : unsigned int {
    START_MOVE = static_cast<unsigned int>(RenderObjectMessage::END),
    END_MOVE, START_PRIMARY, START_SECONDARY, END_PRIMARY, END_SECONDARY, SECONDARY_L, SECONDARY_R, PRIMARY_SET, SELECT_KLASS
};

enum class PlayerRenderMessage : unsigned int {
    TRANSFER = static_cast<unsigned int>(RenderObjectMessage::END),
    ADD_ITEM, FOR_ITEM, SET_PRIMARY, SET_SECONDARY, LEVEL_UP
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
            sf::Keyboard::I, sf::Keyboard::K, sf::Keyboard::J, sf::Keyboard::L,
            sf::Keyboard::Enter, sf::Keyboard::RBracket,
            sf::Keyboard::O, sf::Keyboard::LBracket, sf::Keyboard::P,
            sf::Color(0, 128, 0)
    }},
};

enum class PlayerKlass : unsigned int {
    UNDECIDED, BEGIN,
    BOMBER=BEGIN, ROCKETEER, RADIANT, FLAMER, HEAVY,
    END
};

constexpr unsigned int NUM_KLASSES = static_cast<unsigned int>(PlayerKlass::END) - static_cast<unsigned int>(PlayerKlass::BEGIN);

struct KlassInfo {
    string name, icon;
    multimap<unsigned int, tuple<function<void(shared_ptr<Player>)>, string>> upgrades;
};

map<PlayerKlass, KlassInfo> klass_info = {
    {PlayerKlass::BOMBER, {"Bomber", "data/images/bomber_icon.png", {
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<BombItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<CrateItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<MineDetectorItem>());}, "Basics"}},
        {2, {[](shared_ptr<Player> pl){pl->add_item(make_shared<ShieldItem>());}, "Shield"}},
        {3, {[](shared_ptr<Player> pl){pl->add_item(make_shared<MineItem>());}, "Mines"}},
        {4, {[](shared_ptr<Player> pl){auto i = pl->item<MineDetectorItem>(); i->set_range(i->range() + STANDARD_OBJECT_SIZE);}, "+1 mine detector range"}},
        {5, {[](shared_ptr<Player> pl){auto i = pl->item<BombItem>(); i->set_max_uses(i->max_uses() + 1);}, "+1 bombs"}},
        {6, {[](shared_ptr<Player> pl){auto i = pl->item<CrateItem>(); i->set_hp(i->hp() * 2);}, "x2 crate hp"}},
        {7, {[](shared_ptr<Player> pl){auto i = pl->item<BombItem>(); i->set_size(i->size() * 2);}, "+1 bomb size"}},
        {8, {[](shared_ptr<Player> pl){auto i = pl->item<MineItem>(); i->set_size(i->size() * 2);}, "+1 mine size"}},
        {9, {[](shared_ptr<Player> pl){auto i = pl->item<ShieldItem>(); i->set_max_uses(i->max_uses() * 2);}, "x2 shield strength"}},
        {10, {[](shared_ptr<Player> pl){auto i = pl->item<MineItem>(); i->set_max_uses(i->max_uses() * 2);}, "x2 mines"}},
    }}},
    {PlayerKlass::ROCKETEER, {"Rocketeer", "data/images/rocketeer_icon.png", {
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<BombItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<CrateItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<MineDetectorItem>());}, "Basics"}},
        {2, {[](shared_ptr<Player> pl){pl->add_item(make_shared<ShieldItem>());}, "Shield"}},
        {3, {[](shared_ptr<Player> pl){pl->add_item(make_shared<BurstRocketItem>());}, "Burst Rockets"}},
        {4, {[](shared_ptr<Player> pl){pl->add_item(make_shared<RocketItem>());}, "Rockets"}},
        {5, {[](shared_ptr<Player> pl){auto i = pl->item<RocketItem>(); i->set_max_uses(i->max_uses() + 1);}, "+1 rockets"}},
        {6, {[](shared_ptr<Player> pl){auto i = pl->item<BurstRocketItem>(); i->set_max_uses(i->max_uses() * 2);}, "x2 burst rockets"}},
        {7, {[](shared_ptr<Player> pl){auto i = pl->item<RocketItem>(); i->set_max_uses(i->max_uses() + 1);}, "+1 rockets"}},
        {8, {[](shared_ptr<Player> pl){auto i = pl->item<RocketItem>(); i->set_range(i->range() + 1);}, "+1 rocket range"}},
        {9, {[](shared_ptr<Player> pl){auto i = pl->item<RocketItem>(); i->set_max_uses(i->max_uses() + 1);}, "+1 rockets"}},
        {10, {[](shared_ptr<Player> pl){auto i = pl->item<BurstRocketItem>(); i->set_max_uses(i->max_uses() * 2);}, "x2 burst rockets"}},
    }}},
    {PlayerKlass::RADIANT, {"Radiant", "data/images/radiant_icon.png", {
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<BombItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<CrateItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<MineDetectorItem>());}, "Basics"}},
        {2, {[](shared_ptr<Player> pl){pl->add_item(make_shared<ShieldItem>());}, "Shield"}},
        {3, {[](shared_ptr<Player> pl){pl->add_item(make_shared<LaserItem>());}, "Laser"}},
        {5, {[](shared_ptr<Player> pl){auto i = pl->item<LaserItem>(); i->set_max_uses(i->max_uses() * 2);}, "x2 laser"}},
        {7, {[](shared_ptr<Player> pl){auto i = pl->item<LaserItem>(); i->set_max_uses(i->max_uses() * 2);}, "x2 laser"}},
        {8, {[](shared_ptr<Player> pl){auto i = pl->item<LaserItem>(); i->set_range(i->range() * 2);}, "x2 laser range"}},
        {9, {[](shared_ptr<Player> pl){auto i = pl->item<LaserItem>(); i->set_max_uses(i->max_uses() * 2);}, "x2 laser"}},
        {10, {[](shared_ptr<Player> pl){auto i = pl->item<LaserItem>(); i->set_damage(i->damage() * 2);}, "x2 laser damage"}},
    }}},
    {PlayerKlass::FLAMER, {"Flamer", "data/images/flamer_icon.png", {
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<BombItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<CrateItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<MineDetectorItem>());}, "Basics"}},
        {2, {[](shared_ptr<Player> pl){pl->add_item(make_shared<ShieldItem>());}, "Shield"}},
    }}},
    {PlayerKlass::HEAVY, {"Heavy", "data/images/heavy_icon.png", {
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<BombItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<CrateItem>());}, "Basics"}},
        {1, {[](shared_ptr<Player> pl){pl->add_item(make_shared<MineDetectorItem>());}, "Basics"}},
        {2, {[](shared_ptr<Player> pl){pl->add_item(make_shared<ShieldItem>());}, "Shield"}},
    }}},
};

Player::Player(unsigned int id_, Map* map_) : Object(id_, map_), klass_(PlayerKlass::UNDECIDED), current_selected_klass(PlayerKlass::BOMBER) {
    if (render_map()) {
        destroyed.connect([this] {
            render_map()->add_effect<DeadPlayer>(center(), orientation());
        });
        side_changed.connect([this] {
            setup_keys();
        });
    }
    else {
        auto fid = map->paused.connect([this] {
            direction_stack.clear();
            if (items_.count(primary_item)) {
                items_[primary_item]->try_end();
            }
            if (items_.count(secondary_item)) {
                items_[secondary_item]->try_end();
            }
        });
        destroyed.connect([this, fid] {
            map->paused.disconnect(fid);
        });
    }
}

void Player::post_constructor() {
    Object::post_constructor();
    if (server_map()) {
        set_primary(BombItem::TYPE);
        set_secondary(CrateItem::TYPE);
    }
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

    for (auto& item : items_) {
        item.second->render_overlay(rt);
    }
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
        if (ready()) {
            m["type"] = as_ui(PlayerServerMessage::SECONDARY_L);
        }
        else {
            unsigned int c = static_cast<unsigned int>(current_selected_klass) - static_cast<unsigned int>(PlayerKlass::BEGIN);
            unsigned int n = (c + NUM_KLASSES - 1) % NUM_KLASSES;
            current_selected_klass = static_cast<PlayerKlass>(n + static_cast<unsigned int>(PlayerKlass::BEGIN));
            return;
        }
    }
    else if (key == settings.sec_r) {
        if (!is_down) return;
        if (ready()) {
            m["type"] = as_ui(PlayerServerMessage::SECONDARY_R);
        }
        else {
            unsigned int c = static_cast<unsigned int>(current_selected_klass) - static_cast<unsigned int>(PlayerKlass::BEGIN);
            unsigned int n = (c + 1) % NUM_KLASSES;
            current_selected_klass = static_cast<PlayerKlass>(n + static_cast<unsigned int>(PlayerKlass::BEGIN));
            return;
        }
    }
    else if (key == settings.pri_set) {
        if (!is_down) return;
        if (ready()) {
            m["type"] = as_ui(PlayerServerMessage::PRIMARY_SET);
        }
        else {
            m["type"] = as_ui(PlayerServerMessage::SELECT_KLASS);
            m["klass"] = as_ui(current_selected_klass);
            klass_ = current_selected_klass;
        }
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
            if (!map->is_paused() && items_.count(primary_item)) {
                items_[primary_item]->try_start();
            }
            break;
        }
        case PlayerServerMessage::END_PRIMARY: {
            if (items_.count(primary_item)) {
                items_[primary_item]->try_end();
            }
            break;
        }
        case PlayerServerMessage::START_SECONDARY: {
            if (!map->is_paused() && primary_item != secondary_item && items_.count(secondary_item)) {
                items_[secondary_item]->try_start();
            }
            break;
        }
        case PlayerServerMessage::END_SECONDARY: {
            if (primary_item != secondary_item && items_.count(secondary_item)) {
                items_[secondary_item]->try_end();
            }
            break;
        }
        case PlayerServerMessage::SECONDARY_L: {
            auto prev = lower_bound(items_.begin(), items_.end(), secondary_item, [](auto& a, auto& b){return a.first < b;});
            unsigned int val = -1;
            if (prev == items_.end()) {
                if (items_.size()) {
                    val = (--items_.end())->first;
                }
            }
            else if (prev->first == secondary_item) {
                if (prev == items_.begin()) {
                    val = (--items_.end())->first;
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
            auto next = upper_bound(items_.begin(), items_.end(), secondary_item, [](auto& a, auto& b){return a < b.first;});
            unsigned int val = -1;
            if (next == items_.end()) {
                if (items_.size()) {
                    val = items_.begin()->first;
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
        case PlayerServerMessage::SELECT_KLASS: {
            klass_ = static_cast<PlayerKlass>(m["klass"].as_uint64_t());
            add_upgrades_for_level(0, true);
            on_ready.emit();
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
            level_ = m["level"].as_uint64_t();
            current_selected_klass = static_cast<PlayerKlass>(m["current_selected_klass"].as_uint64_t());
            break;
        }
        case PlayerRenderMessage::ADD_ITEM: {
            auto type = m["item"].as_uint64_t();
            auto item = items_[type] = load_player_items()[type]();
            item->attach(dynamic_pointer_cast<Player>(shared_from_this()));
            break;
        }
        case PlayerRenderMessage::FOR_ITEM: {
            auto type = m["item"].as_uint64_t();
            if (items_.count(type)) {
                items_[type]->render_handle(std::move(m));
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
        case PlayerRenderMessage::LEVEL_UP: {
            level_ = m["level"].as_uint64_t();
            for (auto iter = klass_info[klass_].upgrades.upper_bound(level_ - 1); iter != klass_info[klass_].upgrades.upper_bound(level_); ++iter) {
                render_map()->add_effect<PopupText>(side(), get<1>(iter->second), sf::Color::Red);
            }
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

void Player::add_upgrades_for_level(unsigned int start, bool initial) {
    for (auto iter = klass_info[klass_].upgrades.upper_bound(start); iter != klass_info[klass_].upgrades.upper_bound(level_); ++iter) {
        get<0>(iter->second)(dynamic_pointer_cast<Player>(shared_from_this()));
    }
}

void Player::update() {
    for (auto& item : items_) {
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
    pl->level_ = level_;
    pl->add_upgrades_for_level(0, true);
    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
    m["type"] = as_ui(PlayerRenderMessage::TRANSFER);
    m["id"] = pl->id;
    m["lives"] = pl->lives_;
    m["primary"] = pl->primary_item;
    m["secondary"] = pl->secondary_item;
    m["level"] = pl->level_;
    m["current_selected_klass"] = as_ui(klass_);
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

    sf::Text txt("Lvl " + to_string(level_) + " " + klass_info[klass_].name, rm->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(238, -3);
    rt.draw(txt);

    if (klass_ == PlayerKlass::UNDECIDED) {
        unsigned int h = STANDARD_OBJECT_SIZE + 20;
        unsigned int w = STANDARD_OBJECT_SIZE + 30;
        int x = rt.getView().getSize().x / 2 - w * (static_cast<unsigned int>(PlayerKlass::END) - static_cast<unsigned int>(PlayerKlass::BEGIN)) / 2;
        int y = rt.getView().getSize().y - static_cast<int>(h) - 10;

        for (PlayerKlass k = PlayerKlass::BEGIN; k != PlayerKlass::END; k = static_cast<PlayerKlass>(static_cast<unsigned int>(k) + 1)) {
            sf::Vector2f pos(x + w / 2, y + 3);
            if (k == current_selected_klass) {
                sf::RectangleShape bg(sf::Vector2f(STANDARD_OBJECT_SIZE + 4, STANDARD_OBJECT_SIZE + 4));
                bg.setOrigin((STANDARD_OBJECT_SIZE + 4) / 2, 0);
                bg.setPosition(pos - sf::Vector2f(0, 2));
                bg.setFillColor(sf::Color(255, 255, 255, 128));
                rt.draw(bg);
            }

            sf::Sprite spr(render_map()->load_texture(klass_info[k].icon));
            spr.setOrigin(spr.getTextureRect().width / 2.0f, 0.0f);
            spr.setPosition(pos);
            rt.draw(spr);

            sf::Text text(klass_info[k].name, render_map()->load_font("data/fonts/font.pcf"), 12);
            sf::FloatRect textRect = text.getLocalBounds();
            text.setOrigin(textRect.left + textRect.width/2.0f,
                           textRect.top  + textRect.height/2.0f);
            text.setPosition(pos + sf::Vector2f(0, (h + STANDARD_OBJECT_SIZE) / 2));
            text.setFillColor(sf::Color(255, 0, 0));
            rt.draw(text);

            x += w;
        }
    }

    if (map->is_paused()) {
        return;
    }

    sf::RectangleShape lowerbg(sf::Vector2f(rt.getView().getSize().x, STANDARD_OBJECT_SIZE + 6));
    lowerbg.setFillColor(sf::Color(0, 0, 0, 128));
    lowerbg.setPosition(0, rt.getView().getSize().y - lowerbg.getSize().y);
    rt.draw(lowerbg);

    int x = 3;

    for (auto& item : items_) {
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
        item->attach(dynamic_pointer_cast<Player>(shared_from_this()));
        if (items_.count(item->type())) {
            items_[item->type()]->merge_with(item);
        }
        else {
            items_.emplace(item->type(), item);
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

unsigned int Player::take_damage(unsigned int damage, DamageType dt) {
    if (items_.count(primary_item) && items_[primary_item]->active()) {
        damage = items_[primary_item]->damage_intercept(damage, dt);
    }
    if (items_.count(secondary_item) && items_[secondary_item]->active()) {
        damage = items_[secondary_item]->damage_intercept(damage, dt);
    }
    return Object::take_damage(damage, dt);
}

void Player::level_up() {
    ++level_;
    add_upgrades_for_level(level_ - 1, false);

    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
    m["id"] = id;
    m["type"] = as_ui(PlayerRenderMessage::LEVEL_UP);
    m["level"] = level_;
    server_map()->event(shared_from_this(), std::move(m));
}

bool Player::ready() {
    return klass_ != PlayerKlass::UNDECIDED;
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
