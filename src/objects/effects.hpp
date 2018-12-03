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

#ifndef EFFECTS_HPP
#define EFFECTS_HPP

#include "../effect.hpp"
#include <string>

class PopupText : public Effect {
public:
    unsigned int layer() override;
    PopupText(RenderMap* map_, unsigned int id_, unsigned int side_, std::string txt, sf::Color color_=sf::Color::Black);
    void render(sf::RenderTarget& rt) override;
    void update() override;
private:
    unsigned int time_left = 60;
    sf::Text text;
    unsigned int side;
    sf::Color color;
};

#endif // EFFECTS_HPP

