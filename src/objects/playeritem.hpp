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
    bool active_ = false;
protected:
    std::shared_ptr<Player> player;
public:
    void attach(std::shared_ptr<Player> pl);
    void drop();
    inline bool active() {
        return active_;
    }
    virtual void render(sf::RenderTarget& rt, sf::Vector2f position) = 0;
    virtual unsigned int type() = 0;
    virtual void start();
    virtual void end();
    virtual void merge_with(std::shared_ptr<PlayerItem> item);
    virtual void render_handle(msgpackvar&& m);
};

class UsesPlayerItem : public PlayerItem {
protected:
    unsigned int uses;
    void send_update();
public:
    UsesPlayerItem(unsigned int uses_);
    void merge_with(std::shared_ptr<PlayerItem> item) override;
    void render_handle(msgpackvar && m) override;
};

class BombItem : public UsesPlayerItem {
public:
    BombItem();
    constexpr static const int TYPE = 0;
    virtual unsigned int type() override {
        return 0;
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
};

class CrateItem : public UsesPlayerItem {
public:
    CrateItem();
    constexpr static const int TYPE = 1;
    virtual unsigned int type() override {
        return 1;
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
};

class MineItem : public UsesPlayerItem {
public:
    MineItem();
    constexpr static const int TYPE = 2;
    virtual unsigned int type() override {
        return 2;
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
};

std::map<unsigned int, std::function<std::shared_ptr<PlayerItem>()>> load_player_items();

#endif // PLAYERITEM_HPP