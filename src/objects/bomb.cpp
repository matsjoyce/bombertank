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
#include <random>
#include <iostream>

using namespace std;

void StaticBomb::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/bomb.png"));
    position_sprite(sp);
    rt.draw(sp);
}

pair<unsigned int, bool> kill_in_tile(ServerMap* sm, int x, int y, objptr obj, unsigned int damage) {
    bool all_dead = true;
    // TODO use one collide box and iterate though until no damage left (objs are now sorted by distance)
    for (auto& obj2 : sm->collides(x + 5, y + 5, obj->width - 10, obj->height - 10)) {
        damage = obj2.second->take_damage(damage, DamageType::FORCE);
        all_dead = all_dead && !obj2.second->alive();
    }
    return {damage, all_dead};
}

int kill_in_direction(ServerMap* sm, objptr obj, Orientation::Orientation ori, int range, int damage) {
    int i = 1;
    for (; i <= range; ++i) {
        auto [new_dmg, all_dead] = kill_in_tile(sm, obj->x() + i * dx(ori) * STANDARD_OBJECT_SIZE, obj->y() + i * dy(ori) * STANDARD_OBJECT_SIZE, obj, damage);
        damage = new_dmg;
        if (!damage) {
            if (all_dead) {
                ++i;
            }
            break;
        }
    }

    return i - 1;
}

void StaticBomb::destroy(bool send /*= true*/) {
    if (!alive()) return;
    if (auto sm = server_map()) {
        Object::destroy(false);
        msgpackvar m;
        m["mtype"] = as_ui(ToRenderMessage::FOROBJ);
        m["type"] = as_ui(RenderObjectMessage::DESTROY);
        m["id"] = id;
        kill_in_tile(sm, x(), y(), shared_from_this(), 100);
        m["n"] = kill_in_direction(sm, shared_from_this(), Orientation::N, power, 100);
        m["e"] = kill_in_direction(sm, shared_from_this(), Orientation::E, power, 100);
        m["s"] = kill_in_direction(sm, shared_from_this(), Orientation::S, power, 100);
        m["w"] = kill_in_direction(sm, shared_from_this(), Orientation::W, power, 100);
        sm->event(shared_from_this(), std::move(m));
    }
    else {
        Object::destroy(send);
    }
}

void add_sides(RenderMap* rm, Orientation::Orientation ori, int num, int x, int y) {
    for (int i = 1; i <= num; ++i) {
        rm->add_effect<Explosion>(x + i * dx(ori) * STANDARD_OBJECT_SIZE, y + i * dy(ori) * STANDARD_OBJECT_SIZE, ori, i == num ? Explosion::Position::END : Explosion::Position::MIDDLE);
    }
}

void StaticBomb::render_handle(msgpackvar m) {
    if (m["type"].as_uint64_t() == as_ui(RenderObjectMessage::DESTROY)) {
        if (auto rm = render_map()) {
            rm->add_effect<Explosion>(x(), y());
            add_sides(rm, Orientation::N, m["n"].as_uint64_t(), x(), y());
            add_sides(rm, Orientation::E, m["e"].as_uint64_t(), x(), y());
            add_sides(rm, Orientation::S, m["s"].as_uint64_t(), x(), y());
            add_sides(rm, Orientation::W, m["w"].as_uint64_t(), x(), y());
        }
        destroy();
    }
    else {
        Object::render_handle(m);
    }
}

unsigned int StaticBomb::layer() {
    return 4;
}

void TimedBomb::update() {
    --time_left;
    if (!time_left) {
        destroy();
    }
}

random_device rd;
mt19937 generator(rd());
uniform_int_distribution<int> distribution(0, 3);

RoboBomb::RoboBomb(unsigned int id_, Map* map_) : StaticBomb(id_, map_) {
    set_direction(Orientation::Orientation(distribution(generator)));
}

void RoboBomb::update() {
    if (auto sm = server_map()) {
        if (wait) {
            --wait;
            return;
        }
        else if (stuck) {
            auto old_dir = direction();
            while (direction() == old_dir) {
                set_direction(Orientation::Orientation(distribution(generator)));
            }
            set_orientation(direction());
            stuck = false;
        }

        accelerate(2 - speed());
    }
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

void Mine::collision(objptr obj, bool caused_by_self) {
    if (obj->side() != side()) {
        destroy();
    }
}

void Mine::render(sf::RenderTarget& rt) {
    auto rm = render_map();
    if (rm->side() == side()) {
        sf::Sprite sp(render_map()->load_texture("data/images/mine.png"));
        position_sprite(sp);
        rt.draw(sp);
    }
}

Explosion::Explosion(RenderMap* map_, unsigned int id_, int x_, int y_, Orientation::Orientation ori, Position pos/* = CENTER*/)
    : Effect(map_, id_, x_, y_, ori), position(pos) {
    if (pos == Position::CENTER) {
        sound.setBuffer(map->load_sound_buf("data/sounds/explosion.ogg"));
        sound.play();
    }
}

void Explosion::render(sf::RenderTarget& rt) {
    if (time_left) {
        sf::Sprite sp(map->load_texture(position == Position::CENTER ? "data/images/explosion.png"
                                      : position == Position::MIDDLE ? "data/images/explosion_middle.png" : "data/images/explosion_side.png"));
        sp.setOrigin(sf::Vector2f(STANDARD_OBJECT_SIZE / 2, STANDARD_OBJECT_SIZE / 2));
        sp.setPosition(sf::Vector2f(x + STANDARD_OBJECT_SIZE / 2, y + STANDARD_OBJECT_SIZE / 2));
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
