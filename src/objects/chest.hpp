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

#ifndef CHEST_HPP
#define CHEST_HPP

#include "loader.hpp"

class Chest : public Object {
public:
    Chest(unsigned int id_, Map* map_);
    constexpr static const int TYPE = 8;
    virtual unsigned int type() override {
        return 8;
    }
    void render(sf::RenderTarget& rt) override;
    unsigned int render_layer() override;
    void render_handle(msgpackvar m) override;
    void collision(objptr obj, bool caused_by_self) override;
};

class LevelUp : public Object {
public:
    constexpr static const int TYPE = 12;
    virtual unsigned int type() override {
        return 12;
    }
    using Object::Object;
    void render(sf::RenderTarget& rt) override;
    unsigned int render_layer() override;
    void update() override;
    unsigned int layer() override;
    bool show_in_editor() override {
        return false;
    }
};

#endif // CHEST_HPP
