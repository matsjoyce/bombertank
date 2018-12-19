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

#ifndef MAP_HPP
#define MAP_HPP

#include <map>
#include <memory>
#include <set>
#include <random>
#include <functional>
#include "eventqueue.hpp"
#include "object.hpp"
#include "signal.hpp"

enum class ToRenderMessage : unsigned int {
    CREATE,
    FOROBJ,
    PAUSED,
    RESUMED
};

enum class ToServerMessage : unsigned int {
    PAUSE,
    RESUME,
    FOROBJ
};

class Map {
protected:
    Map();
    virtual ~Map() = default;
    Map(Map&&) = default;
    std::map<unsigned int, std::function<objptr(unsigned int, Map*)>> object_creators;
    std::map<unsigned int, objptr> objects;
    objptr add(unsigned int type, unsigned int id);
    bool is_paused_ = false, is_editor_ = false;
    std::mt19937 generator;
public:
    Signal<> paused;
    Signal<> resumed;
    inline bool is_paused() const {
        return is_paused_;
    }
    std::mt19937& random_generator();
    virtual void remove(objptr obj);
    bool is_editor() {
        return is_editor_;
    }
    void set_is_editor(bool ie) {
        is_editor_ = ie;
    }
};

#endif // MAP_HPP
