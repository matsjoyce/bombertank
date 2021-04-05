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

#include "bomb.hpp"
#include "player.hpp"
#include "attackutils.hpp"
#include "playeritem.hpp"
#include <iostream>

using namespace std;

void StaticBomb::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/charge.png"));
    position_sprite(sp);
    rt.draw(sp);
}

StaticBomb::StaticBomb(unsigned int id_, Map* map_) : Object(id_, map_) {
    auto sm = server_map();
    if (sm && !sm->is_editor()) {
        destroyed.connect([sm, this]{
            msgpackvar m;
            m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
            m["type"] = as_ui(RenderObjectMessage::END);
            m["id"] = id;
            m["n"] = progressive_kill_in_direction(sm, center(), 14, STANDARD_OBJECT_SIZE * power, Orientation::N, damage, DamageType::FORCE, -1) / STANDARD_OBJECT_SIZE;
            m["e"] = progressive_kill_in_direction(sm, center(), 14, STANDARD_OBJECT_SIZE * power, Orientation::E, damage, DamageType::FORCE, -1) / STANDARD_OBJECT_SIZE;
            m["s"] = progressive_kill_in_direction(sm, center(), 14, STANDARD_OBJECT_SIZE * power, Orientation::S, damage, DamageType::FORCE, -1) / STANDARD_OBJECT_SIZE;
            m["w"] = progressive_kill_in_direction(sm, center(), 14, STANDARD_OBJECT_SIZE * power, Orientation::W, damage, DamageType::FORCE, -1) / STANDARD_OBJECT_SIZE;
            sm->event(shared_from_this(), std::move(m));
        });
    }
}

void StaticBomb::set_damage(unsigned int dmg) {
    damage = dmg;
}

void add_sides(RenderMap* rm, Orientation::Orientation ori, int num, Point p) {
    for (int i = 1; i <= num; ++i) {
        rm->add_effect<Explosion>(p + Point(ori) * i * STANDARD_OBJECT_SIZE, ori, i == num ? Explosion::Position::END : Explosion::Position::MIDDLE);
    }
}

void StaticBomb::render_handle(msgpackvar m) {
    switch(static_cast<RenderObjectMessage>(m["type"].as_uint64_t())) {
        case RenderObjectMessage::END: {
            if (auto rm = render_map()) {
                rm->add_effect<Explosion>(center());
                add_sides(rm, Orientation::N, m["n"].as_uint64_t(), center());
                add_sides(rm, Orientation::E, m["e"].as_uint64_t(), center());
                add_sides(rm, Orientation::S, m["s"].as_uint64_t(), center());
                add_sides(rm, Orientation::W, m["w"].as_uint64_t(), center());
            }
            destroy();
            break;
        }
        default: Object::render_handle(m);
    }
}

unsigned int StaticBomb::render_layer() {
    return 4;
}

void StaticBomb::set_power(unsigned int pwr) {
    power = pwr;
}

void TimedBomb::update() {
    --time_left;
    if (!time_left) {
        destroy();
    }
}

void TimedBomb::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/bomb.png"));
    position_sprite(sp);
    rt.draw(sp);
}

RoboBomb::RoboBomb(unsigned int id_, Map* map_) : StaticBomb(id_, map_) {
    if (server_map()) {
        uniform_int_distribution<int> distribution(0, 3);
        set_direction(Orientation::Orientation(distribution(map->random_generator())));
        destroyed.connect([this]{
            server_map()->level_up_trigger(shared_from_this());
        });
    }
}

void RoboBomb::update() {
    if (wait) {
        --wait;
        return;
    }
    else if (stuck) {
        uniform_int_distribution<int> distribution(0, 3);
        auto old_dir = direction();
        while (direction() == old_dir) {
            set_direction(Orientation::Orientation(distribution(map->random_generator())));
        }
        set_orientation(direction());
        stuck = false;
    }

    accelerate(2 - speed());
}

void RoboBomb::collision(objptr obj, bool caused_by_self) {
    if (obj->type() == Player::TYPE) {
        destroy();
    }
    else if (caused_by_self) {
        stuck = true;
        wait = 20;
    }
}


void RoboBomb::render(sf::RenderTarget& rt) {
    auto t = clock.getElapsedTime().asSeconds();
    if (t > 0.2 && speed()) {
        clock.restart();
    }
    sf::Sprite sp(render_map()->load_texture(t < 0.1 && speed() ? "data/images/robobomb2.png" : "data/images/robobomb.png"));
    position_sprite(sp);
    rt.draw(sp);
}

void Mine::update() {
    for (auto& obj : server_map()->collides(*this)) {
        if (obj.second->side() != side() && !obj.second->is_projectile()) {
            destroy();
        }
    }
}

void Mine::render(sf::RenderTarget& rt) {
    auto rm = render_map();
    if (/*rm->side() == side() || */rm->is_editor()) {
        sf::Sprite sp(render_map()->load_texture("data/images/mine.png"));
        position_sprite(sp);
        rt.draw(sp);
    }
}

unsigned int Mine::layer() {
    return 4;
}

Explosion::Explosion(RenderMap* map_, unsigned int id_, Point pos_, Orientation::Orientation ori, Position pos/* = CENTER*/)
    : Effect(map_, id_, pos_, ori), position(pos) {
    if (pos == Position::CENTER) {
        sound.setBuffer(map->load_sound_buf("data/sounds/explosion.ogg"));
        sound.play();
    }
}

void Explosion::render(sf::RenderTarget& rt) {
    if (time_left) {
        sf::Sprite sp(map->load_texture(position == Position::CENTER ? "data/images/explosion.png"
                                      : position == Position::MIDDLE ? "data/images/explosion_middle.png" : "data/images/explosion_side.png"));
        sp.setOrigin(sf::Vector2f(sp.getTextureRect().width / 2, sp.getTextureRect().height / 2));
        sp.setPosition(pos);
        sp.setRotation(angle(orientation));
        sp.setColor(sf::Color(255, 255, 255, min(255, time_left * 25)));
        rt.draw(sp);
    }
}

void Explosion::update() {
    if (time_left) {
        --time_left;
    }
    if (!time_left && sound.getStatus() == sf::Sound::Stopped) {
        destroy();
    }
}

unsigned int Explosion::layer() {
    return position == Position::CENTER ? 10 : 9;
}

LaserRobo::LaserRobo(unsigned int id_, Map* map_) : Object(id_, map_) {
    if (server_map()) {
        uniform_int_distribution<int> distribution(0, 3);
        set_direction(Orientation::Orientation(distribution(map->random_generator())));
        destroyed.connect([this]{
            server_map()->level_up_trigger(shared_from_this());
        });
    }
}

void LaserRobo::update() {
    if (!check_laser()) {
        check_speed();
    }
}

void LaserRobo::check_speed() {
    if (wait) {
        --wait;
        return;
    }
    else if (stuck) {
        uniform_int_distribution<int> distribution(0, 3);
        auto old_dir = direction();
        while (direction() == old_dir) {
            set_direction(Orientation::Orientation(distribution(map->random_generator())));
        }
        set_orientation(direction());
        stuck = false;
    }

    accelerate(2 - speed());
}

bool LaserRobo::check_laser() {
    if (warmup) {
        --warmup;
    }
    else {
        for (unsigned int direction = 0; direction < 4; ++direction){
            auto r = Rect(Size(4, range_).rotate(Orientation::Orientation(direction)));
            auto start = dir_center(Orientation::Orientation(direction));
            r.set_dir_center(opposite(Orientation::Orientation(direction)), start);
            auto dist_r = Rect(start, start);
            for (auto& obj : server_map()->collides(r, [&dist_r](objptr obj){return obj->separation_distance(dist_r);})) {
                if (obj.second->type() == Player::TYPE) {
                    fire_laser(Orientation::Orientation(direction));
                    set_orientation(Orientation::Orientation(direction));
                    accelerate(-speed());
                    _generate_move();
                    wait = 10;
                    stuck = true;
                    return true;
                }
                else if (obj.second->id != id) {
                    break;
                }
            }
        }
    }
    return false;
}

void LaserRobo::fire_laser(Orientation::Orientation direction) {
    auto sm = server_map();
    warmup = 5;

    auto pos = dir_center(direction);
    auto dist = progressive_kill_in_direction(sm, pos, 4, range_, direction, damage_, DamageType::HEAT, layer());

    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
    m["type"] = as_ui(RenderObjectMessage::END);
    m["id"] = id;
    m["dist"] = dist;
    m["ori"] = as_ui(direction);
    m["x"] = pos.x;
    m["y"] = pos.y;
    sm->event(shared_from_this(), std::move(m));
}

void LaserRobo::render_handle(msgpackvar m) {
    switch(static_cast<RenderObjectMessage>(m["type"].as_uint64_t())) {
        case RenderObjectMessage::END: {
            if (auto rm = render_map()) {
                rm->add_effect<LaserEffect>(Point(extract_int(m["x"]), extract_int(m["y"])),
                                            static_cast<Orientation::Orientation>(m["ori"].as_uint64_t()), m["dist"].as_uint64_t());
            }
            break;
        }
        default: Object::render_handle(m);
    }
}

void LaserRobo::collision(objptr obj, bool caused_by_self) {
    stuck = true;
    wait = 20;
}

void LaserRobo::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/laserrobo.png"));
    position_sprite(sp);
    rt.draw(sp);
}

unsigned int LaserRobo::render_layer() {
    return 4;
}

void LaserTurret::update() {
    check_laser();
}

void LaserTurret::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/laserturret.png"));
    position_sprite(sp);
    rt.draw(sp);
}

unsigned int LaserTurret::max_hp() {
    return 50;
}
