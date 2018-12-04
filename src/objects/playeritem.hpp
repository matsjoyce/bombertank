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

class PlayerItem : public std::enable_shared_from_this<PlayerItem> {
    bool active_ = false;
    std::weak_ptr<Player> player_;
protected:
    std::shared_ptr<Player> player();
public:
    Signal<> on_attach;
    Signal<> on_detatch;

    virtual ~PlayerItem() = default;
    void attach(std::shared_ptr<Player> pl);
    void drop();
    inline bool active() {
        return active_;
    }
    virtual unsigned int type() = 0;
    virtual std::string name() = 0;
    virtual void make_empty();

    virtual bool can_activate();

    void try_start();
    void try_end();

    virtual void start();
    virtual void end();
    virtual void merge_with(std::shared_ptr<PlayerItem> item);
    virtual void update();
    virtual unsigned int damage_intercept(unsigned int damage, DamageType dt);

    virtual void render(sf::RenderTarget& rt, sf::Vector2f position) = 0;
    virtual void render_overlay(sf::RenderTarget& rt);
    virtual void render_handle(msgpackvar&& m);
};

class UsesPlayerItem : public PlayerItem {
protected:
    unsigned int uses = 0;
    void send_update();
public:
    UsesPlayerItem();
    void merge_with(std::shared_ptr<PlayerItem> item) override;
    void render_handle(msgpackvar&& m) override;
    bool can_activate() override;
    virtual unsigned int max_uses() = 0;
    void make_empty() override;
};

class UsedPlayerItem : public PlayerItem {
protected:
    unsigned int used = 0;
    void send_update();
public:
    void render_handle(msgpackvar&& m) override;
    bool can_activate() override;
    virtual unsigned int max_uses() = 0;
};

class BombItem : public UsedPlayerItem {
public:
    constexpr static const int TYPE = 0;
    unsigned int type() override {
        return 0;
    }
    std::string name() override {
        return "Bombs";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
    unsigned int max_uses() override;
};

class CrateItem : public UsedPlayerItem {
public:
    constexpr static const int TYPE = 1;
    unsigned int type() override {
        return 1;
    }
    std::string name() override {
        return "Crates";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
    unsigned int max_uses() override;
};

class MineItem : public UsesPlayerItem {
public:
    constexpr static const int TYPE = 2;
    unsigned int type() override {
        return 2;
    }
    std::string name() override {
        return "Mines";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
    unsigned int max_uses() override;
};

class ChargeItem : public UsesPlayerItem {
public:
    constexpr static const int TYPE = 3;
    unsigned int type() override {
        return 3;
    }
    std::string name() override {
        return "Charges";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
    unsigned int max_uses() override;
};

class LaserItem : public UsesPlayerItem {
    unsigned int warmup = 0;
public:
    constexpr static const int TYPE = 4;
    unsigned int type() override {
        return 4;
    }
    std::string name() override {
        return "Laser";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void update() override;
    void render_handle(msgpackvar&& m) override;
    unsigned int max_uses() override;
};

class ShieldItem : public UsesPlayerItem {
    int glow = 0;
public:
    constexpr static const int TYPE = 5;
    unsigned int type() override {
        return 5;
    }
    std::string name() override {
        return "Shield";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    unsigned int damage_intercept(unsigned int damage, DamageType dt);
    void render_handle(msgpackvar&& m) override;
    void render_overlay(sf::RenderTarget& rt) override;
    unsigned int max_uses() override;
};

class RocketItem : public UsesPlayerItem {
public:
    constexpr static const int TYPE = 6;
    unsigned int type() override {
        return 6;
    }
    std::string name() override {
        return "Rockets";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
    unsigned int max_uses() override;
};


std::map<unsigned int, std::function<std::shared_ptr<PlayerItem>()>> load_player_items();

#endif // PLAYERITEM_HPP
