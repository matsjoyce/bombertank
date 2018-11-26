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

EventServer::EventServer() : rt(thread{[this]{return run();}}) {
}

EventServer::~EventServer() {
    push(Message());
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
    Message m;
    while (true) {
        {
            unique_lock<mutex> ul(qmu);
            cv.wait(ul, [this]{return !qu.empty();});
            m = move(qu.front());
            qu.pop();
        }
        switch (m.type()) {
            case Message::HALT: return;
            default: {
                lock_guard<mutex> lg(mu);
                buf.emplace_back(move(m));
            }
        }
    }
}
