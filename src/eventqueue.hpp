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

#ifndef EVENTQUEUE_HPP
#define EVENTQUEUE_HPP

#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <thread>
#include "msgpack_utils.hpp"


class EventServer {
    std::mutex mu;
    std::vector<msgpackvar> buf;
//     std::thread rt;

    EventServer* other;
    inline void push(msgpackvar&& m) {
        std::lock_guard<std::mutex> lock(mu);
        buf.emplace_back(std::forward<msgpackvar>(m));
    }
public:
    EventServer();
    ~EventServer();
    inline void send(msgpackvar&& m) {
        other->push(std::forward<msgpackvar>(m));
    }
    std::vector<msgpackvar> events();
//     void run();
    inline void connect(EventServer* s) {
        other = s;
    }
};

#endif // EVENTQUEUE_HPP
