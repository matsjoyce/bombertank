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

#include "eventqueue.hpp"

using namespace std;

void EventQueue::push(Message && m) {
    {
        lock_guard<mutex> lock(mu);
        qu.push(m);
    }
    cv.notify_one();
}

Message EventQueue::pop() {
    unique_lock<mutex> ul(mu);
    cv.wait(ul, [this]{return !qu.empty();});
    Message m = move(qu.front());
    qu.pop();
    return m;
}

EventServer::EventServer(EventPipe& seq, EventPipe& req) : send_eq(seq), recv_eq(req), rt(thread{[this]{return run();}}) {
}

EventServer::~EventServer() {
    recv_eq.push(Message());
    rt.join();
}


vector<Message> EventServer::events() {
    lock_guard<mutex> lg(mu);
    auto ret = move(buf);
    buf = {};
    return ret;
}

void EventServer::run() {
    cout << "EventServer::run is go!" << endl;
    while (true) {
        auto m = recv_eq.pop();
        switch (m.type()) {
            case Message::HALT: return;
            default: {
                lock_guard<mutex> lg(mu);
                buf.emplace_back(move(m));
            }
        }
    }
}
