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

class PlayerItem;

enum class PlayerKlass : unsigned int;

class Player : public Object {
    std::vector<Orientation::Orientation> direction_stack;
    int max_speed = 4;
    unsigned int level_ = 10;
    unsigned int lives_ = 3;
    sf::Clock anim_clock;
    std::string tex_name = "data/images/tank1.png";
    void setup_keys();
    std::map<unsigned int, std::shared_ptr<PlayerItem>> items_;
    unsigned int active_item = -1;
    void add_upgrades_for_level(unsigned int start, bool initial);
    PlayerKlass klass_;
    unsigned int shield_ = -1, max_shield_ = 25, shield_glow = 0, klass_select_wait = 200;
public:
    Signal<> on_ready;
    void post_constructor() override;
    constexpr static const int TYPE = 3;
    virtual unsigned int type() override {
        return 3;
    }
    inline unsigned int lives() const {
        return lives_;
    }
    inline unsigned int level() const {
        return level_;
    }
    inline const std::map<unsigned int, std::shared_ptr<PlayerItem>>& items() const {
        return items_;
    }
    unsigned int render_layer() override;
    Player(unsigned int id_, Map* map_);
    void render(sf::RenderTarget& rt) override;
    void handle_keypress(sf::Keyboard::Key key, bool is_down) override;
    void handle(msgpackvar m) override;
    void render_handle(msgpackvar m) override;
    void update() override;
    unsigned int take_damage(unsigned int damage, DamageType dt) override;
    unsigned int max_hp() override;
    void transfer(objptr obj);
    void add_item(std::shared_ptr<PlayerItem> item);
    void render_hud(sf::RenderTarget & rt) override;
    void item_msg(msgpackvar&& m, unsigned int type);
    void level_up();
    bool ready();
    template<class T> std::shared_ptr<T> item() {
        return std::dynamic_pointer_cast<T>(items_[T::TYPE]);
    }
    unsigned int shield() const {
        return shield_;
    }
    void set_shield(unsigned int shield);
    unsigned int max_shield() const {
        return max_shield_;
    }
    void set_max_shield(unsigned int max_shield);
};

class DeadPlayer : public Effect {
public:
    unsigned int layer() override;
    using Effect::Effect;
    void render(sf::RenderTarget & rt) override;
};

#endif // PLAYER_HPP
