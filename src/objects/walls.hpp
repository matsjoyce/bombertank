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

#ifndef WALLS_HPP
#define WALLS_HPP

#include "loader.hpp"

class Wall : public Object {
public:
    constexpr static const int TYPE = 0;
    virtual unsigned int type() override {
        return 0;
    }
    using Object::Object;
    unsigned int layer() override;
    void render(sf::RenderTarget& rt) override;
    unsigned int max_hp() override;
};

class IndestructableWall : public Object {
public:
    constexpr static const int TYPE = 1;
    virtual unsigned int type() override {
        return 1;
    }
    using Object::Object;
    unsigned int layer() override;
    void render(sf::RenderTarget& rt) override;
    unsigned int take_damage(unsigned int damage, DamageType dt) override;
};

class PlacedWall : public Object {
public:
    constexpr static const int TYPE = 2;
    virtual unsigned int type() override {
        return 2;
    }
    using Object::Object;
    void render(sf::RenderTarget& rt) override;
    unsigned int max_hp() override;
    unsigned int layer() override;
};

#endif // WALLS_HPP
