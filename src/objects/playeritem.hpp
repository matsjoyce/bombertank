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

#ifndef PLAYERITEM_HPP
#define PLAYERITEM_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "player.hpp"

class PlayerItem {
protected:
    std::shared_ptr<Player> player;
public:
    void attach(std::shared_ptr<Player> pl);
    void drop();
    virtual void render(sf::RenderTarget& rt, sf::Vector2f position) = 0;
    virtual unsigned int type() = 0;
    virtual void use() = 0;
    virtual void merge_with(std::shared_ptr<PlayerItem> item) = 0;
    virtual void render_handle(msgpackvar&& m) = 0;
};

class BombItem : public PlayerItem {
    int uses = 3;
public:
    constexpr static const int TYPE = 0;
    virtual unsigned int type() override {
        return 0;
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void use() override;
    void merge_with(std::shared_ptr<PlayerItem> item) override;
    void render_handle(msgpackvar && m) override;
};

class CrateItem : public PlayerItem {
    int uses = 8;
public:
    constexpr static const int TYPE = 1;
    virtual unsigned int type() override {
        return 1;
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void use() override;
    void merge_with(std::shared_ptr<PlayerItem> item) override;
    void render_handle(msgpackvar && m) override;
};

std::map<unsigned int, std::function<std::shared_ptr<PlayerItem>()>> load_player_items();

#endif // PLAYERITEM_HPP
