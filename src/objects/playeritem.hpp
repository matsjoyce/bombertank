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
    unsigned int uses = 0, max_uses_ = 1;
    void send_update();
public:
    UsesPlayerItem(unsigned int max_uses=1);
    void merge_with(std::shared_ptr<PlayerItem> item) override;
    void render_handle(msgpackvar&& m) override;
    bool can_activate() override;
    unsigned int max_uses() const {
        return max_uses_;
    }
    void set_max_uses(unsigned int max_uses) {
        auto diff = max_uses - max_uses_;
        max_uses_ = max_uses;
        uses += diff;
        send_update();

    }
    void make_empty() override;
};

class UsedPlayerItem : public PlayerItem {
protected:
    unsigned int used = 0, max_uses_ = 1;
    void send_update();
public:
    UsedPlayerItem(unsigned int max_uses=1);
    void render_handle(msgpackvar&& m) override;
    bool can_activate() override;
    unsigned int max_uses() const {
        return max_uses_;
    }
    void set_max_uses(unsigned int max_uses) {
        max_uses_ = max_uses;
        send_update();
    }
};

class BombItem : public UsedPlayerItem {
    unsigned int damage_ = 100, size_ = 1;
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
    unsigned int damage() const {
        return damage_;
    }
    void set_damage(unsigned int damage) {
        damage_ = damage;
    }
    unsigned int size() const {
        return size_;
    }
    void set_size(unsigned int size) {
        size_ = size;
    }
};

class CrateItem : public UsedPlayerItem {
    unsigned int hp_ = 50;
public:
    CrateItem() : UsedPlayerItem(3) {}
    constexpr static const int TYPE = 1;
    unsigned int type() override {
        return 1;
    }
    std::string name() override {
        return "Crates";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
    unsigned int hp() const {
        return hp_;
    }
    void set_hp(unsigned int hp) {
        hp_ = hp;
    }
};

class MineItem : public UsesPlayerItem {
    unsigned int damage_ = 100, size_ = 1;
public:
    MineItem() : UsesPlayerItem(3) {}
    constexpr static const int TYPE = 2;
    unsigned int type() override {
        return 2;
    }
    std::string name() override {
        return "Mines";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
    unsigned int damage() const {
        return damage_;
    }
    void set_damage(unsigned int damage) {
        damage_ = damage;
    }
    unsigned int size() const {
        return size_;
    }
    void set_size(unsigned int size) {
        size_ = size;
    }
};

class ChargeItem : public UsesPlayerItem {
    unsigned int damage_ = 100, size_ = 1;
public:
    ChargeItem() : UsesPlayerItem(2) {}
    constexpr static const int TYPE = 3;
    unsigned int type() override {
        return 3;
    }
    std::string name() override {
        return "Charges";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
    unsigned int damage() const {
        return damage_;
    }
    void set_damage(unsigned int damage) {
        damage_ = damage;
    }
    unsigned int size() const {
        return size_;
    }
    void set_size(unsigned int size) {
        size_ = size;
    }
};

class LaserItem : public UsesPlayerItem {
    unsigned int warmup = 0, range_ = STANDARD_OBJECT_SIZE * 5, damage_ = 10;
public:
    LaserItem() : UsesPlayerItem(30) {}
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
    unsigned int range() const {
        return range_;
    }
    void set_range(unsigned int range) {
        range_ = range;
    }
    unsigned int damage() const {
        return damage_;
    }
    void set_damage(unsigned int damage) {
        damage_ = damage;
    }
};

class LaserEffect : public Effect {
    unsigned int dist;
public:
    unsigned int layer() override {
        return 5;
    }
    LaserEffect(RenderMap* map_, unsigned int id_, Point pos_, Orientation::Orientation ori, unsigned int dist_);
    void render(sf::RenderTarget& rt) override;
    void update() override;
private:
    int time_left = 5;
    sf::Sound sound;
};

class RocketItem : public UsesPlayerItem {
    unsigned int range_ = 20, damage_ = 25;
public:
    RocketItem() : UsesPlayerItem(3) {}
    constexpr static const int TYPE = 6;
    unsigned int type() override {
        return 6;
    }
    std::string name() override {
        return "Rockets";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void start() override;
    unsigned int range() const {
        return range_;
    }
    void set_range(unsigned int range) {
        range_ = range;
    }
    unsigned int damage() const {
        return damage_;
    }
    void set_damage(unsigned int damage) {
        damage_ = damage;
    }
};

class BurstRocketItem : public UsesPlayerItem {
    unsigned int warmup = 0, range_ = 20, damage_ = 20;
    bool left_side = true;
public:
    BurstRocketItem() : UsesPlayerItem(12) {}
    constexpr static const int TYPE = 8;
    unsigned int type() override {
        return 8;
    }
    std::string name() override {
        return "Burst Rockets";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void update() override;
    unsigned int range() const {
        return range_;
    }
    void set_range(unsigned int range) {
        range_ = range;
    }
    unsigned int damage() const {
        return damage_;
    }
    void set_damage(unsigned int damage) {
        damage_ = damage;
    }
};

class MineDetectorItem : public UsesPlayerItem {
    unsigned int range_ = STANDARD_OBJECT_SIZE * 5 / 2;
public:
    MineDetectorItem() : UsesPlayerItem(10) {}
    constexpr static const int TYPE = 7;
    unsigned int type() override {
        return 7;
    }
    std::string name() override {
        return "Mine Detector";
    }
    void render(sf::RenderTarget& rt, sf::Vector2f position) override;
    void render_handle(msgpackvar&& m) override;
    void start() override;
    unsigned int range() const {
        return range_;
    }
    void set_range(unsigned int range) {
        range_ = range;
    }
};


std::map<unsigned int, std::function<std::shared_ptr<PlayerItem>()>> load_player_items();

#endif // PLAYERITEM_HPP
