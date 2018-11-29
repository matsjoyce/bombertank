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

#ifndef POINT_HPP
#define POINT_HPP

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <ostream>
#include "orientation.hpp"

constexpr const int STANDARD_OBJECT_SIZE = 24;

constexpr int int_floor_div(int a, int b) {
    // From https://stackoverflow.com/a/46270859/3946766
    int d = a / b;
    int r = a % b;
    return r ? (d - ((a < 0) ^ (b < 0))) : d;
}

inline void inline_int_floor_div(int& a, int b) {
    // From https://stackoverflow.com/a/46270859/3946766
    int d = a / b;
    int r = a % b;
    a = r ? (d - ((a < 0) ^ (b < 0))) : d;
}

template<class T, class Q> class Vec {
public:
    T x = 0, y = 0;
    Vec() = default;
    Vec(T x_, T y_) : x(x_), y(y_) {}
    Vec(Orientation::Orientation ori) : x(dx(ori)), y(dy(ori)) {}

    // Inline Vec operators

    inline Q& operator+=(const Q& p) {
        x += p.x;
        y += p.y;
        return *this;
    }
    inline Q& operator-=(const Q& p) {
        x -= p.x;
        y -= p.y;
        return *this;
    }
    inline Q& operator*=(const Q& p) {
        x *= p.x;
        y *= p.y;
        return *this;
    }
    inline Q& operator/=(const Q& p) {
        inline_floor_div(x, p.x);
        inline_floor_div(y, p.y);
        return *this;
    }
    inline Q& operator%=(const Q& p) {
        x %= p.x;
        y %= p.y;
        return *this;
    }

    // Inline Int operators

    inline Q& operator+=(int p) {
        x += p;
        y += p;
        return *this;
    }
    inline Q& operator-=(int p) {
        x -= p;
        y -= p;
        return *this;
    }
    inline Q& operator*=(int p) {
        x *= p;
        y *= p;
        return *this;
    }
    inline Q& operator/=(int p) {
        inline_floor_div(x, p);
        inline_floor_div(y, p);
        return *this;
    }
    inline Q& operator%=(int p) {
        x %= p;
        y %= p;
        return *this;
    }

    // Vec operators

    inline Q operator+(const Q& p) const {
        return {x + p.x, y + p.y};
    }
    inline Q operator-(const Q& p) const {
        return {x - p.x, y - p.y};
    }
    inline Q operator*(const Q& p) const {
        return {x * p.x, y * p.y};
    }
    inline Q operator/(const Q& p) const {
        return {int_floor_div(x, p.x), int_floor_div(y, p.y)};
    }
    inline Q operator%(const Q& p) const {
        return {x % p.x, y % p.y};
    }

    // Int operators

    inline Q operator+(int p) const {
        return {x + p, y + p};
    }
    inline Q operator-(int p) const {
        return {x - p, y - p};
    }
    inline Q operator*(int p) const {
        return {x * p, y * p};
    }
    inline Q operator/(int p) const {
        return {int_floor_div(x, p), int_floor_div(y, p)};
    }
    inline Q operator%(int p) const {
        return {x % p, y % p};
    }

    // Comparison

    inline bool operator==(const Q& p) {
        return x == p.x && y == p.y;
    }

    inline bool operator!=(const Q& p) {
        return x != p.x || y != p.y;
    }

    // Conversion

    operator sf::Vector2i() {
        return {x, y};
    }

    operator sf::Vector2f() {
        return {static_cast<float>(x), static_cast<float>(y)};
    }
};

class Size : public Vec<unsigned int, Size> {
public:
    using Vec<unsigned int, Size>::Vec;
    Size(const sf::Vector2u& p) : Vec<unsigned int, Size>(p.x, p.y) {}

    // Rotation

    inline Size rotate(Orientation::Orientation ori) {
        switch (ori) {
            case Orientation::N:
            case Orientation::S: return {x, y};
            case Orientation::E:
            case Orientation::W: return {y, x};
        }
    }
};

class Point : public Vec<int, Point> {
public:
    using Vec<int, Point>::Vec;
    Point(const sf::Vector2u& p) : Vec<int, Point>(p.x, p.y) {}
    Point(const sf::Vector2i& p) : Vec<int, Point>(p.x, p.y) {}

    inline Point to_tile() {
        return (*this) / STANDARD_OBJECT_SIZE;
    }
    inline Point from_tile() {
        return (*this) * STANDARD_OBJECT_SIZE;
    }
    inline Size to_size() {
        return {static_cast<unsigned int>(std::abs(x)), static_cast<unsigned int>(std::abs(y))};
    }

    // Rotation

    inline Point rotate(Orientation::Orientation ori) {
        switch (ori) {
            case Orientation::N: return {x, y};
            case Orientation::W: return {-y, x};
            case Orientation::S: return {-x, -y};
            case Orientation::E: return {y, -x};
        }
    }
};

std::ostream& operator<<(std::ostream& stream, const Point& p);
std::ostream& operator<<(std::ostream& stream, const Size& s);

#endif // POINT_HPP
