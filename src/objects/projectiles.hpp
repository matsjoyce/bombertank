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
};

#endif // PROJECTILES_HPP
