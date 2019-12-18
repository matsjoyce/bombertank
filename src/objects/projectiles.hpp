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

#ifndef PROJECTILES_HPP
#define PROJECTILES_HPP

#include "loader.hpp"
#include "bomb.hpp"

class Rocket : public StaticBomb {
    unsigned int time_left = 20;
    sf::Sound sound;
public:
    constexpr static const int TYPE = 10;
    virtual unsigned int type() override {
        return 10;
    }
    Rocket(unsigned int id_, Map* map_);
    void update() override;
    void render(sf::RenderTarget& rt) override;
    unsigned int render_layer() override;
    void collision(objptr obj, bool caused_by_self) override;
    void set_range(unsigned int r) {
        time_left = r;
    }
    bool is_projectile() override {
        return true;
    }
};

class MiniRocket : public Object {
    unsigned int time_left = 20;
    sf::Sound sound;
    unsigned int power = 1;
    unsigned int damage = 100;
public:
    constexpr static const int TYPE = 11;
    virtual unsigned int type() override {
        return 11;
    }
    MiniRocket(unsigned int id_, Map* map_);
    void update() override;
    void render(sf::RenderTarget& rt) override;
    void render_handle(msgpackvar m) override;
    unsigned int render_layer() override;
    void collision(objptr obj, bool caused_by_self) override;
    void set_range(unsigned int r) {
        time_left = r;
    }
    void set_damage(unsigned int d) {
        damage = d;
    }
    void set_power(unsigned int p) {
        power = p;
    }
    bool is_projectile() override {
        return true;
    }
};

#endif // PROJECTILES_HPP
