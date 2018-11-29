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

#ifndef RECT_HPP
#define RECT_HPP

#include <algorithm>
#include <ostream>
#include "point.hpp"
#include "orientation.hpp"

class Rect {
    int x_ = 0, y_ = 0;
    unsigned int width_ = 0, height_ = 0;
    // Used for object moving detection
    inline void internal_on_change() {
        dirty = true;
    }
protected:
    bool dirty = false;
public:
    Rect(unsigned int w, unsigned int h) : width_(w), height_(h) {}
    Rect(Size s) : Rect(s.x, s.y) {}
    Rect(Point a, Point b) : x_(std::min(a.x, b.x)), y_(std::min(a.y, b.y)), width_(std::abs(a.x - b.x)), height_(std::abs(a.y - b.y)) {}

    // Center

    inline Point center() const {
        return {x_ + static_cast<int>(width_ / 2), y_ + static_cast<int>(height_ / 2)};
    }
    inline void set_center(int x, int y) {
        x_ = x - static_cast<int>(width_ / 2);
        y_ = y - static_cast<int>(height_ / 2);
        internal_on_change();
    }
    inline void set_center(const Point& p) {
        set_center(p.x, p.y);
    }

    // Corners

    inline Point nw_corner() const {
        return {x_, y_};
    }
    inline void set_nw_corner(int x, int y) {
        x_ = x;
        y_ = y;
        internal_on_change();
    }
    inline void set_nw_corner(const Point& p) {
        set_nw_corner(p.x, p.y);
    }

    inline Point ne_corner() const {
        return {x_ + static_cast<int>(width_), y_};
    }
    inline void set_ne_corner(int x, int y) {
        x_ = x - static_cast<int>(width_);
        y_ = y;
        internal_on_change();
    }
    inline void set_ne_corner(const Point& p) {
        set_ne_corner(p.x, p.y);
    }

    inline Point se_corner() const {
        return {x_ + static_cast<int>(width_), y_ + static_cast<int>(height_)};
    }
    inline void set_se_corner(int x, int y) {
        x_ = x - static_cast<int>(width_);
        y_ = y - static_cast<int>(height_);
        internal_on_change();
    }
    inline void set_se_corner(const Point& p) {
        set_se_corner(p.x, p.y);
    }

    inline Point sw_corner() const {
        return {x_, y_ + static_cast<int>(height_)};
    }
    inline void set_sw_corner(int x, int y) {
        x_ = x;
        y_ = y - static_cast<int>(height_);
        internal_on_change();
    }
    inline void set_sw_corner(const Point& p) {
        set_sw_corner(p.x, p.y);
    }

    inline Point left_corner(Orientation::Orientation ori) const {
        switch (ori) {
            case Orientation::N: return ne_corner();
            case Orientation::E: return nw_corner();
            case Orientation::S: return se_corner();
            case Orientation::W: return sw_corner();
        }
    }
    inline void set_left_corner(Orientation::Orientation ori, int x, int y) {
        switch (ori) {
            case Orientation::N: return set_ne_corner(x, y);
            case Orientation::E: return set_nw_corner(x, y);
            case Orientation::S: return set_se_corner(x, y);
            case Orientation::W: return set_sw_corner(x, y);
        }
    }
    inline void set_left_corner(Orientation::Orientation ori, const Point& p) {
        set_left_corner(ori, p.x, p.y);
    }

    inline Point right_corner(Orientation::Orientation ori) const {
        switch (ori) {
            case Orientation::N: return nw_corner();
            case Orientation::E: return se_corner();
            case Orientation::S: return sw_corner();
            case Orientation::W: return ne_corner();
        }
    }
    inline void set_right_corner(Orientation::Orientation ori, int x, int y) {
        switch (ori) {
            case Orientation::N: return set_nw_corner(x, y);
            case Orientation::E: return set_se_corner(x, y);
            case Orientation::S: return set_sw_corner(x, y);
            case Orientation::W: return set_ne_corner(x, y);
        }
    }
    inline void set_right_corner(Orientation::Orientation ori, const Point& p) {
        set_right_corner(ori, p.x, p.y);
    }

    // Line centers

    inline Point n_center() const {
        return {x_ + static_cast<int>(width_ / 2), y_};
    }
    inline void set_n_center(int x, int y) {
        x_ = x - static_cast<int>(width_ / 2);
        y_ = y;
        internal_on_change();
    }
    inline void set_n_center(const Point& p) {
        set_n_center(p.x, p.y);
    }

    inline Point e_center() const {
        return {x_ + static_cast<int>(width_), y_ + static_cast<int>(height_ / 2)};
    }
    inline void set_e_center(int x, int y) {
        x_ = x - static_cast<int>(width_);
        y_ = y - static_cast<int>(height_ / 2);
        internal_on_change();
    }
    inline void set_e_center(const Point& p) {
        set_e_center(p.x, p.y);
    }

    inline Point s_center() const {
        return {x_ + static_cast<int>(width_ / 2), y_ + static_cast<int>(height_)};
    }
    inline void set_s_center(int x, int y) {
        x_ = x - static_cast<int>(width_ / 2);
        y_ = y - static_cast<int>(height_);
        internal_on_change();
    }
    inline void set_s_center(const Point& p) {
        set_s_center(p.x, p.y);
    }

    inline Point w_center() const {
        return {x_, y_ + static_cast<int>(height_ / 2)};
    }
    inline void set_w_center(int x, int y) {
        x_ = x;
        y_ = y - static_cast<int>(height_ / 2);
        internal_on_change();
    }
    inline void set_w_center(const Point& p) {
        set_n_center(p.x, p.y);
    }

    inline Point dir_center(Orientation::Orientation ori) const {
        switch (ori) {
            case Orientation::N: return n_center();
            case Orientation::E: return e_center();
            case Orientation::S: return s_center();
            case Orientation::W: return w_center();
        }
    }
    inline void set_dir_center(Orientation::Orientation ori, int x, int y) {
        switch (ori) {
            case Orientation::N: return set_n_center(x, y);
            case Orientation::E: return set_e_center(x, y);
            case Orientation::S: return set_s_center(x, y);
            case Orientation::W: return set_w_center(x, y);
        }
    }
    inline void set_dir_center(Orientation::Orientation ori, const Point& p) {
        set_dir_center(ori, p.x, p.y);
    }

    // Size

    inline unsigned int width() const {
        return width_;
    }
    inline void set_width(unsigned int w) {
        width_ = w;
    }

    inline unsigned int height() const {
        return height_;
    }
    inline void set_height(unsigned int h) {
        height_ = h;
    }

    inline Size size() const {
        return {width_, height_};
    }
    inline void set_size(unsigned int w, unsigned int h) {
        width_ = w;
        height_ = h;
        internal_on_change();
    }
    inline void set_size(Size s) {
        set_size(s.x, s.y);
    }

    // Movement

    inline void move(int dx, int dy) {
        x_ += dx;
        y_ += dy;
        internal_on_change();
    }
    inline void move(const Point& p) {
        move(p.x, p.y);
    }

    // Collisions

    inline int separation_distance(const Rect& r) {
        return std::max(std::max(r.x_ - x_ - static_cast<int>(width_),
                                 x_ - r.x_ - static_cast<int>(r.width_)),
                        std::max(r.y_ - y_ - static_cast<int>(height_),
                                 y_ - r.y_ - static_cast<int>(r.height_)));
    }

    // Comparisons

    inline bool operator==(const Rect& other) const {
        return x_ == other.x_ && y_ == other.y_ && width_ == other.width_ && height_ == other.height_;
    }

    inline bool operator!=(const Rect& other) const {
        return x_ != other.x_ || y_ != other.y_ || width_ != other.width_ || height_ != other.height_;
    }
};

class RectangularIterator {
    const Rect& rect;
public:
    class inner {
        const Rect& rect;
        unsigned int index;
    public:
        inner(const Rect& r, unsigned int idx) : rect(r), index(idx) {}
        Point operator*() const {
            auto p = rect.nw_corner();
            auto s = rect.size();
            return {p.x + static_cast<int>(index % s.x), p.y + static_cast<int>(index / s.x)};
        }
        bool operator==(const inner& other) const {
            return index == other.index && rect == other.rect;
        }
        bool operator!=(const inner& other) const {
            return index != other.index || rect != other.rect;
        }
        inner& operator++() {
            ++index;
            return *this;
        }
        inner& operator--() {
            --index;
            return *this;
        }
        inner operator++(int) {
            return {rect, index++};
        }
        inner operator--(int) {
            return {rect, index--};
        }
    };

    friend class inner;

    RectangularIterator(const Rect& r) : rect(r) {
    }

    inner begin() const {
        return inner(rect, 0);
    }
    inner end() const {
        auto s = rect.size();
        return inner(rect, s.x * s.y);
    }
};

std::ostream& operator<<(std::ostream& stream, const Rect& r);

#endif // RECT_HPP
