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
#include "../build/src/message.pb.h"

class EventPipe {
public:
    virtual void push(Message&& m) = 0;
    virtual Message pop() = 0;
};

class EventQueue : public EventPipe {
    std::mutex mu;
    std::queue<Message> qu;
    std::condition_variable cv;
public:
    void push(Message&& m) override;
    Message pop() override;
};

class EventServer {
    EventPipe& send_eq;
    EventPipe& recv_eq;

    std::mutex mu;
    std::vector<Message> buf;
    std::thread rt;
public:
    EventServer(EventPipe& seq, EventPipe& req);
    ~EventServer();
    inline void send(Message&& m) {
        send_eq.push(std::forward<Message>(m));
    }
    std::vector<Message> events();
    void run();
};

#endif // EVENTQUEUE_HPP
