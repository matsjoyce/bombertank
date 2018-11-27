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
#include "map.hpp"
#include "rendermap.hpp"
#include "../build/src/generic_msg.pb.h"
#include <cmath>

using namespace std;

const double PI = 3.14159265358979323846;

Object::Object(unsigned int id_, Map* map_) : id(id_), map(map_) {
}

void Object::place(int x, int y) {
    x_ = x;
    y_ = y;
    _generate_move();
}

void Object::place_on_tile(int tx, int ty) {
    place(tx * STANDARD_OBJECT_SIZE, ty * STANDARD_OBJECT_SIZE);
}


void Object::update() {
}

void Object::render(sf::RenderTarget& /*rt*/) {
}

void Object::handle(Message m) {
    cout << "Unhandled event in Object::handle " << m.type() << endl;
}

void Object::render_handle(Message m) {
    switch (m.type()) {
        case Message::MOVE: {
            if (!m.value().Is<GenericMessage>()) {
                throw runtime_error("Move message's value is not a GenericMessage");
            }
            GenericMessage gm;
            m.value().UnpackTo(&gm);
            x_ = gm.x();
            y_ = gm.y();
//             accel_ = gm.accel(); // TODO Is this required?
            speed_ = gm.speed();
            direction_ = Orientation::Orientation(gm.direction());
            orientation_ = Orientation::Orientation(gm.orientation());
            break;
        }
        case Message::DESTROY: {
            set_hp(0);
            destroy();
            break;
        }
        case Message::CHANGE_SIDE: {
            if (!m.value().Is<GenericMessage>()) {
                throw runtime_error("Change side message's value is not a GenericMessage");
            }
            GenericMessage gm;
            m.value().UnpackTo(&gm);
            set_side(gm.side());
            break;
        }
        case Message::TAKE_DAMAGE: {
            if (!m.value().Is<GenericMessage>()) {
                throw runtime_error("Take damage message's value is not a GenericMessage");
            }
            GenericMessage gm;
            m.value().UnpackTo(&gm);
            set_hp(gm.hp());
            break;
        }
        default: {
            cout << "Unhandled event in Object::render_handle " << m.type() << endl;
        }
    }
}

void Object::handle_keypress(sf::Keyboard::Key /*key*/, bool /*is_down*/) {
}

void Object::start_update() {
    accel_ = -speed_;
    hp_ = min(hp_, max_hp());
}


constexpr const int GET_ROUND_MARGIN = STANDARD_OBJECT_SIZE / 3;


void Object::end_update() {
    if (!map) return; // May have been destroyed in update

    speed_ += accel_;
    bool edge_help = false;
    if (speed_) {
        int go_left = 0, go_right = 0, go_total = 0;
        for (auto& obj : server_map()->collides_by_moving(x(), y(), width, height, direction(), speed())) {
            if (obj.first < speed_) {
                speed_ = max(0, obj.first);
            }
            if (obj.first > speed_) {
                break;
            }
            collision(obj.second, true);
            obj.second->collision(shared_from_this(), false);
            if (obj.first == 0) {
                go_right += obj.second->separation_distance(x() + dx(right(direction_)) * GET_ROUND_MARGIN, y() + dy(right(direction_)) * GET_ROUND_MARGIN, width, height, direction_, 1) >= 0;
                go_left += obj.second->separation_distance(x() + dx(left(direction_)) * GET_ROUND_MARGIN, y() + dy(left(direction_)) * GET_ROUND_MARGIN, width, height, direction_, 1) >= 0;
                go_total += 1;
            }
        }

        if (speed_) {
            x_ += dx(direction_) * speed_;
            y_ += dy(direction_) * speed_;
        }
        else if (go_total) {
            auto dir = direction_;
            if (go_left == go_total) {
                dir = left(direction_);
            }
            else if (go_right == go_total) {
                dir = right(direction_);
            }
            if (dir != direction_ && !server_map()->collides_by_moving(x(), y(), width, height, dir, 1).size()) {
                x_ += dx(dir);
                y_ += dy(dir);
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

double Object::angle_to(objptr obj) {
    return atan2(obj->x_ - x_, y_ - obj->y_) * 180 / PI;
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
        Message m;
        m.set_type(Message::MOVE);
        m.set_id(id);
        GenericMessage gm;
        gm.set_x(x_);
        gm.set_y(y_);
    //     gm.set_accel(accel_); // TODO Is this required?
        gm.set_speed(speed_);
        gm.set_direction(direction_);
        gm.set_orientation(orientation_);
        m.mutable_value()->PackFrom(gm);
        sm->event(shared_from_this(), move(m));
    }
}

int Object::separation_distance(objptr obj) {
    return max(max(obj->x_ - x_ - static_cast<int>(width),
                   x_ - obj->x_ - static_cast<int>(obj->width)),
               max(obj->y_ - y_ - static_cast<int>(height),
                   y_ - obj->y_ - static_cast<int>(obj->height)));
}

int Object::separation_distance(int ox, int oy, int ow, int oh) {
    return max(max(ox - x_ - static_cast<int>(width),
                   x_ - ox - static_cast<int>(ow)),
               max(oy - y_ - static_cast<int>(height),
                   y_ - oy - static_cast<int>(oh)));
}

int Object::separation_distance(int ox, int oy, int ow, int oh, Orientation::Orientation dir, int movement) {
    auto dx_ = dx(dir) * movement;
    if (dx_ > 0) {
        ow += dx_;
    }
    else {
        ox += dx_;
        ow -= dx_;
    }
    auto dy_ = dy(dir) * movement;
    if (dy_ > 0) {
        oh += dy_;
    }
    else {
        oy += dy_;
        oh -= dy_;
    }
    return separation_distance(ox, oy, ow, oh);
}


void Object::render_update() {
}

unsigned int Object::take_damage(unsigned int damage, DamageType /*dt*/) {
    cout << id << " taking " << damage << " dmg with " << hp_ << " hp" << endl;
    if (damage >= hp_) {
        damage -= hp_;
        hp_ = 0;
        destroy();
        return damage;
    }
    else {
        hp_ -= damage;
        if (auto sm = server_map()) {
            Message m;
            m.set_type(Message::TAKE_DAMAGE);
            m.set_id(id);
            GenericMessage gm;
            gm.set_hp(hp());
            m.mutable_value()->PackFrom(gm);
            sm->event(shared_from_this(), move(m));
        }
        return 0;
    }
}

void Object::destroy(bool send /*= true*/) {
    if (!map) return; // Already destroyed;
    destroyed.emit();
    if (send) {
        if (auto sm = server_map()) {
            Message m;
            m.set_type(Message::DESTROY);
            m.set_id(id);
            sm->event(shared_from_this(), move(m));
        }
    }
    map->remove(shared_from_this());
    is_alive = false;
}

unsigned int Object::max_hp() {
    return 10;
}

unsigned int Object::layer() {
    return 0;
}

unsigned int Object::type() {
    return -1;
}

void Object::set_side(unsigned int side) {
    side_ = side;
    side_changed.emit();
    if (auto sm = server_map()) {
        Message m;
        m.set_type(Message::CHANGE_SIDE);
        m.set_id(id);
        GenericMessage gm;
        gm.set_side(side);
        m.mutable_value()->PackFrom(gm);
        sm->event(shared_from_this(), move(m));
    }
}

void Object::set_hp(unsigned int hp) {
    hp_ = hp;
}

void Object::collision(objptr /*obj*/, bool /*caused_by_self*/) {
}

void Object::position_sprite(sf::Sprite& spr) {
    spr.setOrigin(sf::Vector2f(width / 2, height / 2));
    spr.setPosition(sf::Vector2f(x_ + width / 2, y_ + height / 2));
    spr.setRotation(angle(orientation_));
}

void Object::render_hud(sf::RenderTarget& rt) {
    auto rm = render_map();
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
