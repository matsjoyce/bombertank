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

#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <functional>
#include <map>
#include <algorithm>

template<class... T> class Signal {
public:
    using func_type = std::function<void(T...)>;
    unsigned int connect(func_type f) {
        auto id = next_id++;
        funcs[id] = f;
        return id;
    }
    void disconnect(unsigned int id) {
        if (executing) {
            funcs[id] = {};
        }
        else {
            funcs.erase(id);
        }
    }
    void emit(T... args) {
        executing = true;
        for (auto iter = funcs.begin(); iter != funcs.end();) {
            if (iter->second) {
                iter->second(args...);
                ++iter;
            }
            else {
                funcs.erase(iter++);
            }
        }
        executing = false;
    }
private:
    unsigned int next_id = 0;
    std::map<unsigned int, func_type> funcs;
    bool executing = false;
};

#endif // SIGNAL_HPP
