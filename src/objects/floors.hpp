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

#ifndef FLOORS_HPP
#define FLOORS_HPP

#include "loader.hpp"

class Lava : public Object {
public:
    using Object::Object;
    constexpr static const int TYPE = 4;
    virtual unsigned int type() override {
        return 4;
    }
    unsigned int render_layer() override;
    void render(sf::RenderTarget& rt) override;
    unsigned int layer() override;
    unsigned int take_damage(unsigned int damage, DamageType dt) override;
    void update() override;
};


#endif // FLOORS_HPP
