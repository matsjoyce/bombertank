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

#include "object.hpp"
#include "servermap.hpp"
#include "rendermap.hpp"
#include <cmath>
#include <iostream>

using namespace std;

const double PI = 3.14159265358979323846;

Object::Object(unsigned int id_, Map* map_) : Rect(STANDARD_OBJECT_SIZE, STANDARD_OBJECT_SIZE), id(id_), map(map_) {
}

void Object::post_constructor() {
    hp_ = max_hp();
    if (auto sm = server_map()) {
        msgpackvar m;
        m["mtype"] = as_ui(ToRenderMessage::CREATE);
        m["id"] = id;
        m["type"] = type();
        m["x"] = nw_corner().x;
        m["y"] = nw_corner().y;
        m["side"] = side_;
        sm->event(shared_from_this(), std::move(m));
    }
}

void Object::update() {
}

void Object::render(sf::RenderTarget& /*rt*/) {
}

void Object::handle(msgpackvar m) {
    cout << "Unhandled event in Object::handle " << m["type"].as_uint64_t() << endl;
}

void Object::render_handle(msgpackvar m) {
    switch (static_cast<RenderObjectMessage>(m["type"].as_uint64_t())) {
        case RenderObjectMessage::MOVE: {

            set_nw_corner(extract_int(m["x"]), extract_int(m["y"]));
//             accel_ = gm.accel(); // TODO Is this required?
            speed_ = extract_int(m["speed"]);
            direction_ = Orientation::Orientation(m["dir"].as_uint64_t());
            orientation_ = Orientation::Orientation(m["ori"].as_uint64_t());
            break;
        }
        case RenderObjectMessage::DESTROY: {
            set_hp(0);
            destroy();
            break;
        }
        case RenderObjectMessage::CHANGE_SIDE: {
            set_side(m["side"].as_uint64_t());
            break;
        }
        case RenderObjectMessage::TAKE_DAMAGE: {
            set_hp(m["hp"].as_uint64_t());
            break;
        }
        default: {
            cout << "Unhandled event in Object::render_handle " << m["type"].as_uint64_t() << endl;
        }
    }
}

void Object::handle_keypress(sf::Keyboard::Key /*key*/, bool /*is_down*/) {
}

void Object::start_update() {
    accel_ = -speed_;
}


constexpr const int GET_ROUND_MARGIN = STANDARD_OBJECT_SIZE / 3;


void Object::end_update() {
    if (!alive()) return; // May have been destroyed in update

    speed_ += accel_;
    bool edge_help = false;
    if (speed_) {
        unsigned int already_moved = 0;
        int go_left = 0, go_right = 0, go_total = 0;
        for (auto& obj : server_map()->collides_by_moving(*this, direction(), speed())) {
            if (obj.second->layer() != layer()) {
                continue;
            }
            if (obj.first < speed_) {
                speed_ = max(0, obj.first);
                auto to_move = obj.first - already_moved;
                already_moved += to_move;
                move(dx(direction_) * to_move, dy(direction_) * to_move);
            }
            if (obj.first > speed_) {
                break;
            }
            collision(obj.second, true);
            obj.second->collision(shared_from_this(), false);
            if (obj.first == 0) {
                go_right += obj.second->separation_distance(moved(Point(right(direction_)) * GET_ROUND_MARGIN).moved(Point(direction_))) >= 0;
                go_left += obj.second->separation_distance(moved(Point(left(direction_)) * GET_ROUND_MARGIN).moved(Point(direction_))) >= 0;
                go_total += 1;
            }
        }

        auto to_move = speed_ - already_moved;
        move(dx(direction_) * to_move, dy(direction_) * to_move);

        if (!speed_ && go_total) {
            auto dir = direction_;
            if (go_left == go_total) {
                dir = left(direction_);
            }
            else if (go_right == go_total) {
                dir = right(direction_);
            }
            if (dir != direction_ && !server_map()->collides_by_moving(*this, dir, 1).size()) {
                move(dx(dir), dy(dir));
                edge_help = true;
            }
        }
    }
    if (speed_ != last_speed || last_orientation != orientation_ || edge_help || speed_) { // TODO remove || speed_ when prediction enabled
        last_speed = speed_;
        last_orientation = orientation_;

        _generate_move();
    }
}

void Object::accelerate(int amount) {
    accel_ = amount;
}

RenderMap* Object::render_map() {
    return dynamic_cast<RenderMap*>(map);
}

ServerMap* Object::server_map() {
    return dynamic_cast<ServerMap*>(map);
}

void Object::set_direction(Orientation::Orientation dir) {
    direction_ = dir;
}

void Object::set_orientation(Orientation::Orientation dir) {
    orientation_ = dir;
}

void Object::_generate_move() {
    if (auto sm = server_map()) {
        msgpackvar m;
        m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
        m["type"] = as_ui(RenderObjectMessage::MOVE);
        m["id"] = id;
        m["x"] = nw_corner().x;
        m["y"] = nw_corner().y;
        m["speed"] = speed_;
        m["dir"] = as_ui(direction_);
        m["ori"] = as_ui(orientation_);
        sm->event(shared_from_this(), std::move(m));
    }
}

int Object::separation_distance(objptr obj) {
    return separation_distance(*obj);
}

void Object::render_update() {
}

unsigned int Object::take_damage(unsigned int damage, DamageType /*dt*/) {
    if (!alive()) return damage; // Prevent recursion in destroy
    cout << id << "[" << type() << "]" << " taking " << damage << " dmg with " << hp_ << " hp" << endl;
    if (damage >= hp_) {
        damage -= hp_;
        hp_ = 0;
        destroy();
        return damage;
    }
    else {
        hp_ -= damage;
        if (auto sm = server_map()) {

            msgpackvar m;
            m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
            m["type"] = as_ui(RenderObjectMessage::TAKE_DAMAGE);
            m["id"] = id;
            m["hp"] = hp_;
            sm->event(shared_from_this(), std::move(m));
        }
        return 0;
    }
}

void Object::destroy() {
    hp_ = 0;
    destroyed.emit();
    if (auto sm = server_map()) {
        msgpackvar m;
        m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
        m["type"] = as_ui(RenderObjectMessage::DESTROY);
        m["id"] = id;
        sm->event(shared_from_this(), std::move(m));
    }
    map->remove(shared_from_this()); // WARNING: DO NOT PUT ANYTHING BELOW THIS LINE! WE MIGHT NOT EXIST ANY MORE.
}

unsigned int Object::max_hp() {
    return 10;
}

unsigned int Object::layer() {
    return 10;
}

unsigned int Object::render_layer() {
    return 0;
}

unsigned int Object::type() {
    return -1;
}

void Object::set_side(unsigned int side) {
    side_ = side;
    side_changed.emit();
    if (auto sm = server_map()) {
        msgpackvar m;
        m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
        m["type"] = as_ui(RenderObjectMessage::CHANGE_SIDE);
        m["id"] = id;
        m["side"] = side;
        sm->event(shared_from_this(), std::move(m));
    }
}

void Object::set_hp(unsigned int hp) {
    hp_ = hp;
}

void Object::collision(objptr /*obj*/, bool /*caused_by_self*/) {
}

void Object::set_speed(int speed) {
    speed_ = speed;
}

void Object::position_sprite(sf::Sprite& spr) {
    spr.setOrigin(sf::Vector2f(spr.getTextureRect().width / 2, spr.getTextureRect().height / 2));
    spr.setPosition(center());
    spr.setRotation(angle(orientation_));
}

void Object::render_hud(sf::RenderTarget& rt) {
    auto rm = render_map();

    sf::RectangleShape darkbg(sf::Vector2f(rt.getView().getSize().x, 10));
    darkbg.setPosition(0, 0);
    darkbg.setFillColor(sf::Color(0, 0, 0, 128));
    rt.draw(darkbg);

    sf::RectangleShape border(sf::Vector2f(202, 8));
    border.setPosition(1, 1);
    border.setFillColor(sf::Color::Black);
    rt.draw(border);

    auto texbg = rm->load_texture("data/images/hp_bar_bg.png");
    texbg.setRepeated(true);
    sf::Sprite sp_bg(texbg);
    sp_bg.setTextureRect(sf::IntRect(0, 0, 200, 6));
    sp_bg.setPosition(2, 2);
    rt.draw(sp_bg);

    auto texfg = rm->load_texture("data/images/hp_bar_fg.png");
    texfg.setRepeated(true);
    sf::Sprite sp_fg(texfg);
    sp_fg.setTextureRect(sf::IntRect(0, 0, min(200u, 200 * hp() / max_hp()), 6));
    sp_fg.setPosition(2, 2);
    rt.draw(sp_fg);
}
