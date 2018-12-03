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

#include "effects.hpp"
#include "../rendermap.hpp"
#include <iostream>

using namespace std;

PopupText::PopupText(RenderMap* map_, unsigned int id_, unsigned int side_, std::string txt, sf::Color color_/*=sf::Color::Black*/)
    : Effect(map_, id_, {}, {}), text(txt, map_->load_font("data/fonts/font.pcf"), 12), side(side_), color(color_) {
}

void PopupText::render(sf::RenderTarget& rt) {
    if (map->side() == side) {
        auto view = rt.getView();
        auto new_view = view;
        new_view.reset(sf::FloatRect(sf::Vector2f(-view.getSize().x / 4, 0), view.getSize() / 2.0f));
        rt.setView(new_view);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,
                    textRect.top  + textRect.height/2.0f);
        auto col = color;
        col.a = static_cast<unsigned int>(color.a) * min(255u, time_left * 20) / 255;
        text.setFillColor(col);
        text.setPosition(0, time_left + new_view.getSize().y - 60);
        rt.draw(text);
        rt.setView(view);
    }
}

void PopupText::update() {
    if (time_left) {
        --time_left;
    }
    if (!time_left) {
        destroy();
    }
}

unsigned int PopupText::layer() {
    return 100;
}
