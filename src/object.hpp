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

constexpr const int STANDARD_OBJECT_SIZE = 24;

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
    END
};

enum class ServerObjectMessage : unsigned int {
    END
};

class Object : public std::enable_shared_from_this<Object> {
public:
    Object(unsigned int id_, Map* map_);
    virtual ~Object() = default;
    virtual void post_constructor();
    const unsigned int id;
    virtual unsigned int layer();
    virtual unsigned int type();
    virtual unsigned int width();
    virtual unsigned int height();

    Signal<> destroyed;
    Signal<> side_changed;

    void place(int x, int y);
    void place_on_tile(int tx, int ty);
    void accelerate(int amount);
    void set_direction(Orientation::Orientation dir);
    void set_orientation(Orientation::Orientation dir);
    void set_side(unsigned int side);
    void set_hp(unsigned int hp);

    void start_update();
    virtual void update();
    void end_update();
    virtual void handle(msgpackvar m);
    virtual void destroy(bool send=true);
    virtual void collision(objptr obj, bool caused_by_self);
    ServerMap* server_map();

    virtual unsigned int render_layer();
    virtual void render_update();
    virtual void render(sf::RenderTarget& rt);
    virtual void render_hud(sf::RenderTarget& rt);
    virtual void render_handle(msgpackvar m);
    virtual void handle_keypress(sf::Keyboard::Key key, bool is_down);
    RenderMap* render_map();

    double angle_to(objptr obj);
    int separation_distance(objptr obj);
    int separation_distance(int ox, int oy, int ow, int oh);
    int separation_distance(int ox, int oy, int ow, int oh, Orientation::Orientation dir, int movement);

    inline int x() {
        return x_;
    }
    inline int y() {
        return y_;
    }
    inline unsigned int side() {
        return side_;
    }
    // Tile x
    inline int tx() {
        return std::round(static_cast<float>(x_) / STANDARD_OBJECT_SIZE);
    }
    // Tile y
    inline int ty() {
        return std::round(static_cast<float>(y_) / STANDARD_OBJECT_SIZE);
    }
    // Tile x converted
    inline int tcx() {
        return tx() * STANDARD_OBJECT_SIZE;
    }
    // Tile y converted
    inline int tcy() {
        return ty() * STANDARD_OBJECT_SIZE;
    }
    inline int speed() {
        return speed_;
    }
    inline int accel() {
        return accel_;
    }
    inline auto direction() {
        return direction_;
    }
    inline auto orientation() {
        return orientation_;
    }
    inline auto hp() {
        return hp_;
    }
    virtual unsigned int take_damage(unsigned int damage, DamageType dt);
    inline bool alive() {
        return is_alive;
    }
    virtual unsigned int max_hp();
    void position_sprite(sf::Sprite& spr);
protected:
    Map* map;
private:
    Orientation::Orientation direction_ = Orientation::N, orientation_ = Orientation::N;
    int accel_ = 0;
    int speed_ = 0;
    int last_speed = 0;
    Orientation::Orientation last_orientation = Orientation::N;
    int x_ = 0, y_ = 0;
    unsigned int hp_ = -1, side_ = -1;
    bool is_alive = true;

    void _generate_move();
};

#endif // OBJECT_HPP
