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
#include <iostream>

using namespace std;

EventServer::EventServer() : rt(thread{[this]{return run();}}) {
}

EventServer::~EventServer() {
    push(msgpackvar{{"halt_now", true}});
    rt.join();
}


vector<msgpackvar> EventServer::events() {
    lock_guard<mutex> lg(mu);
    vector<msgpackvar> ret;
    buf.swap(ret);
    return ret;
}

void EventServer::run() {
    cout << "EventServer::run is go!" << endl;
    msgpackvar m;
    while (true) {
        {
            unique_lock<mutex> ul(qmu);
            cv.wait(ul, [this]{return !qu.empty();});
            m = std::move(qu.front());
            qu.pop();
        }
        if (m.count("halt_now")) {
            return;
        }
        {
            lock_guard<mutex> lg(mu);
            buf.emplace_back(std::move(m));
        }
    }
}
