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

#include "servermap.hpp"
#include "object.hpp"
#include "objects/player.hpp"
#include "objects/dropitem.hpp"
#include <iostream>
#include <chrono>

using namespace std;

void ServerMap::event(objptr obj, msgpackvar&& msg) {
    // HACK should not need thread safety
    lock_guard<recursive_mutex> lg(mutex);
    pending_events.emplace_back(make_pair(obj, std::move(msg)));
}

objptr ServerMap::add(unsigned int type) {
    // HACK should not need thread safety
    lock_guard<recursive_mutex> lg(mutex);
    auto obj = Map::add(type, next_id++);
    return obj;
}

void ServerMap::update(bool update_objs/*=true*/) {
    in_update = true;
    if (!is_paused_ && update_objs) {
        ++frame_;
        auto [start, end] = frame_callbacks.equal_range(frame_);
        for (; start != end; ++start) {
            start->second();
        }
        frame_callbacks.erase(frame_callbacks.begin(), frame_callbacks.upper_bound(frame_));
        for (auto& obj : objects) {
            obj.second->start_update();
            obj.second->update();
            obj.second->end_update();
        }
    }
    in_update = false;
    for (auto id : defered_destroy) {
        remove(objects[id]);
    }
    defered_destroy.clear();
    for (auto& event : pending_events) {
        ev_sender(std::move(event.second));
    }
    pending_events.clear();
}

vector<pair<int, objptr>> ServerMap::collides(const Rect& r, unsigned int layer/*=-1*/) {
    vector<pair<int, objptr>> ret;
    int dist;
    for (auto& obj : objects) {
        if (!defered_destroy.count(obj.first)
            && (layer == static_cast<unsigned int>(-1) || layer == obj.second->layer())
            && (dist = obj.second->separation_distance(r)) < 0) {
            auto item = make_pair(dist, obj.second);
            ret.insert(upper_bound(ret.begin(), ret.end(), item), item);
        }
    }
    return ret;
}

vector<pair<int, objptr>> ServerMap::collides(const Rect& r, std::function<int(objptr)> sortfunc, unsigned int layer/*=-1*/) {
    vector<pair<int, objptr>> ret;
    for (auto& obj : objects) {
        if (!defered_destroy.count(obj.first)
            && (layer == static_cast<unsigned int>(-1) || layer == obj.second->layer())
            && obj.second->separation_distance(r) < 0) {
            auto item = make_pair(sortfunc(obj.second), obj.second);
            ret.insert(upper_bound(ret.begin(), ret.end(), item), item);
        }
    }
    return ret;
}

vector<pair<int, objptr>> ServerMap::collides_by_moving(const Rect& r, Orientation::Orientation dir, int movement, unsigned int layer/*=-1*/) {
    auto mr = r.movement_rect(dir, movement);
    auto margin = -static_cast<int>(r.dimension_in_dir(dir)) / 2;
    vector<pair<int, objptr>> ret;
    int dist;
    for (auto& obj : objects) {
        if (obj.second->alive() && obj.second->separation_distance(mr) < 0
            && (layer == static_cast<unsigned int>(-1) || layer == obj.second->layer())
            && (dist = obj.second->separation_distance(r)) > margin) {
            auto item = make_pair(dist, obj.second);
            ret.insert(upper_bound(ret.begin(), ret.end(), item), item);
        }
    }
    return ret;
}

void ServerMap::remove(objptr obj) {
    if (in_update) {
        defered_destroy.insert(obj->id);
    }
    else {
        Map::remove(obj);
    }
}

void ServerMap::save_objects_to_map(std::ostream& f) {
    for (auto& obj : objects) {
        auto pos = obj.second->nw_corner().to_tile();
        f << obj.second->type() << " " << pos.x << " " << pos.y << endl;
    }
}

void ServerMap::pause(string reason, bool events_too/*=false*/) {
    is_paused_ = true;
    events_paused = events_too;
    paused.emit();
    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::PAUSED);
    m["reason"] = reason;
    ServerMap::event({}, std::move(m));
}


void ServerMap::resume() {
    is_paused_ = false;
    events_paused = false;
    resumed.emit();
    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::RESUMED);
    ServerMap::event({}, std::move(m));
}

std::vector<unsigned int> DROP_OPTIONS = {HealthDropItem::TYPE, SpeedDropItem::TYPE};

void ServerMap::drop_trigger(objptr obj) {
    if (is_editor()) return;
    if (obj->type() != Player::TYPE) {
        uniform_int_distribution<int> distribution(1, 8);
        if (distribution(random_generator()) != 1) {
            return;
        }
    }
    uniform_int_distribution<int> distribution(0, DROP_OPTIONS.size() - 1);
    auto di = add(DROP_OPTIONS[distribution(random_generator())]);
    di->set_center(obj->center());
    di->_generate_move();
}

void ServerMap::on_frame(unsigned int frame, std::function<void()> f) {
    if (frame <= frame_) {
        cout << "Callback for frame " << frame << " while on frame " << frame_ << endl;
    }
    frame_callbacks.emplace(make_pair(frame, f));
}

void ServerMap::handle(msgpackvar&& event) {
    switch (static_cast<ToServerMessage>(event["mtype"].as_uint64_t())) {
        case ToServerMessage::FOROBJ: {
            auto id = event["id"].as_uint64_t();
            if (objects.count(id)) {
                objects[id]->handle(event);
            }
            else {
                cout << "Event for non-existent object " << id << endl;
            }
            break;
        }
        default: {
            cout << "Unhandled event in ServerMap: " << event["type"].as_uint64_t() << endl;
        }
    }
}
