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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <map>
#include <functional>
#include "orientation.hpp"
#include "signal.hpp"
#include <SFML/Graphics.hpp>
#include "msgpack_utils.hpp"
#include "rect.hpp"

class Map;
class RenderMap;
class ServerMap;
class Object;

using objptr =  std::shared_ptr<Object>;

enum class DamageType : unsigned int {
    FORCE,
    HEAT,
    ELECTRIC
};

enum class RenderObjectMessage : unsigned int {
    MOVE,
    DESTROY,
    CHANGE_SIDE,
    TAKE_DAMAGE,
    HEAL,
    END
};

enum class ServerObjectMessage : unsigned int {
    END
};

class Object : public std::enable_shared_from_this<Object>, public Rect {
public:
    Object(unsigned int id_, Map* map_);
    virtual ~Object() = default;
    virtual void post_constructor();

    Signal<> destroyed;
    Signal<> side_changed;

    const unsigned int id;
    virtual unsigned int type();

    inline int accel() const {
        return accel_;
    }
    void accelerate(int amount);

    inline int speed() const {
        return speed_;
    }
    void set_speed(int speed);

    inline auto direction() const {
        return direction_;
    }
    void set_direction(Orientation::Orientation dir);

    inline auto orientation() const {
        return orientation_;
    }
    void set_orientation(Orientation::Orientation dir);

    inline auto hp() const {
        return hp_;
    }
    void set_hp(unsigned int hp);
    virtual unsigned int max_hp();

    inline unsigned int side() const {
        return side_;
    }
    void set_side(unsigned int side);

    virtual unsigned int take_damage(unsigned int damage, DamageType dt);
    virtual void heal(unsigned int healing);
    inline bool alive() {
        return hp_;
    }

    virtual unsigned int layer();
    void start_update();
    virtual void update();
    void end_update();
    virtual void handle(msgpackvar m);
    void destroy();
    virtual void collision(objptr obj, bool caused_by_self);
    using Rect::separation_distance;
    int separation_distance(objptr obj);
    ServerMap* server_map();

    virtual unsigned int render_layer();
    virtual bool is_projectile() {
        return false;
    }
    virtual bool show_in_editor() {
        return true;
    }
    virtual void render_update();
    virtual void render(sf::RenderTarget& rt);
    virtual void render_hud(sf::RenderTarget& rt);
    virtual void render_handle(msgpackvar m);
    virtual void handle_keypress(sf::Keyboard::Key key, bool is_down);
    RenderMap* render_map();
    void position_sprite(sf::Sprite& spr);

    // DEPRECATED
    void _generate_move();
protected:
    Map* map;
private:
    Orientation::Orientation direction_ = Orientation::N, orientation_ = Orientation::N;
    int accel_ = 0;
    int speed_ = 0;
    int last_speed = 0;
    Orientation::Orientation last_orientation = Orientation::N;
    unsigned int hp_ = -1, side_ = -1;
};

#endif // OBJECT_HPP
