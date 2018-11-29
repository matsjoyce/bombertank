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

#ifndef EFFECT_HPP
#define EFFECT_HPP

#include <SFML/Graphics.hpp>
#include "orientation.hpp"
#include "point.hpp"
#include <memory>

class RenderMap;
class Effect;

using effptr = std::shared_ptr<Effect>;

class Effect : public std::enable_shared_from_this<Effect> {
public:
    const unsigned int id;
    Point pos;
    Orientation::Orientation orientation;
    virtual unsigned int layer();
    RenderMap* map;

    Effect(RenderMap* map_, unsigned int id_, Point pos_, Orientation::Orientation ori=Orientation::N);
    virtual ~Effect() = default;

    virtual void update();
    virtual void render(sf::RenderTarget& rt);
    virtual void destroy();
};

#endif // EFFECT_HPP
