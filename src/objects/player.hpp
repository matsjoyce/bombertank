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

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "loader.hpp"

class Player : public Object {
    std::vector<Orientation::Orientation> direction_stack;
    int max_speed = 4;
    int num_bombs = 3;
    int num;
    int num_walls = 5;
    sf::Clock anim_clock;
    std::string tex_name = "data/images/tank1.png";
public:
    constexpr static const int TYPE = 3;
    virtual unsigned int type() override {
        return 3;
    }
    unsigned int layer() override;
    Player(unsigned int id_, Map* map_);
    void render(sf::RenderTarget& rt) override;
    void handle_keypress(sf::Keyboard::Key key, bool is_down) override;
    void handle(Message m) override;
    void render_handle(Message m) override;
    void update() override;
    void set_num(int num);
    unsigned int max_hp() override;
};

class DeadPlayer : public Effect {
public:
    unsigned int layer() override;
    using Effect::Effect;
    void render(sf::RenderTarget & rt) override;
};

#endif // PLAYER_HPP
