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

#ifndef BOMB_HPP
#define BOMB_HPP

#include "loader.hpp"
#include "../effect.hpp"

class StaticBomb : public Object {
    unsigned int power = 1;
public:
    constexpr static const int TYPE = 5;
    virtual unsigned int type() override {
        return 5;
    }
    using Object::Object;
    unsigned int render_layer() override;
    void render(sf::RenderTarget& rt) override;
    void destroy(bool send=true) override;
    void render_handle(msgpackvar m) override;
    void set_power(unsigned int pwr);
};

class TimedBomb : public StaticBomb {
    int time_left = 60;
public:
    constexpr static const int TYPE = 6;
    virtual unsigned int type() override {
        return 6;
    }
    using StaticBomb::StaticBomb;
    void update() override;
    void render(sf::RenderTarget& rt) override;
};

class RoboBomb : public StaticBomb {
    int wait = 0;
    sf::Clock clock;
    bool stuck = false;
public:
    constexpr static const int TYPE = 7;
    virtual unsigned int type() override {
        return 7;
    }
    RoboBomb(unsigned int id_, Map* map_);
    void update() override;
    void render(sf::RenderTarget& rt) override;
    void collision(objptr obj, bool caused_by_self) override;
};

class Mine : public StaticBomb {
public:
    using StaticBomb::StaticBomb;
    constexpr static const int TYPE = 9;
    virtual unsigned int type() override {
        return 9;
    }
    void render(sf::RenderTarget& rt) override;
    void update() override;
    unsigned int layer() override;
};

class Explosion : public Effect {
public:
    enum class Position {CENTER, MIDDLE, END};
    unsigned int layer() override;
    Explosion(RenderMap* map_, unsigned int id_, Point pos_, Orientation::Orientation ori=Orientation::N, Position pos = Position::CENTER);
    void render(sf::RenderTarget& rt) override;
    void update() override;
private:
    int time_left = 15;
    Position position;
    sf::Sound sound;
};

#endif // BOMB_HPP
