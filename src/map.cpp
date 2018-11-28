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

#include <chrono>
#include "map.hpp"
#include "object.hpp"
#include "objects/loader.hpp"
#include "objects/player.hpp"
#include <iostream>

using namespace std;

Map::Map() : object_creators(load_objects()) {
}

objptr Map::add(unsigned int type, unsigned int id) {
    return objects[id] = object_creators[type](id, this);
}

void Map::remove(objptr obj) {
    objects.erase(obj->id);
}


void ServerMap::add_controller(unsigned int side, std::unique_ptr<EventServer> es) {
    side_controllers.emplace(make_pair(side, move(es)));
}

void ServerMap::event(objptr obj, msgpackvar&& msg) {
    pending_events.emplace_back(make_pair(obj, std::move(msg)));
}

objptr ServerMap::add(unsigned int type) {
    auto obj = Map::add(type, next_id++);
    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::CREATE);
    m["id"] = obj->id;
    m["type"] = type;
    m["x"] = obj->x();
    m["y"] = obj->y();
    m["side"] = obj->side();
    event(obj, std::move(m));
    return obj;
}

void ServerMap::update() {
    in_update = true;
    for (auto& obj : objects) {
        obj.second->start_update();
        obj.second->update();
        obj.second->end_update();
    }
    in_update = false;
    for (auto id : defered_destroy) {
        remove(objects[id]);
    }
    defered_destroy.clear();
    for (auto& event : pending_events) {
        for (auto& sc : side_controllers) {
            auto m = event.second;
            sc.second->send(std::move(m));
        }
    }
    pending_events.clear();
}

void ServerMap::run() {
    is_running = true;
    while (is_running) {
        auto start = chrono::high_resolution_clock::now();

        for (auto& sc : side_controllers) {
            for (auto event : sc.second->events()) {
                switch (static_cast<ToServerMessage>(event["mtype"].as_uint64_t())) {
                    case ToServerMessage::PAUSE: {
                        pause();
                        break;
                    }
                    case ToServerMessage::RESUME: {
                        resume();
                        break;
                    }
                    case ToServerMessage::FOROBJ: {
                        auto id = event["id"].as_uint64_t();
                        if (is_paused_) {
                            cout << "Event for obj " << id << " while paused!" << endl;
                        }
                        else if (objects.count(id)) {
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
        }
        if (!is_paused_) {
            update();
        }
        auto end = chrono::high_resolution_clock::now();
        this_thread::sleep_for(std::chrono::milliseconds(50) - (end - start));
    }
}

void ServerMap::halt() {
    is_running = false;
}

vector<pair<int, objptr>> ServerMap::collides(int ox, int oy, int ow, int oh) {
    vector<pair<int, objptr>> ret;
    int dist;
    for (auto& obj : objects) {
        if (!defered_destroy.count(obj.first) && (dist = obj.second->separation_distance(ox, oy, ow, oh)) < 0) {
            auto item = make_pair(dist, obj.second);
            ret.insert(upper_bound(ret.begin(), ret.end(), item), item);
        }
    }
    return ret;
}

vector<pair<int, objptr>> ServerMap::collides_by_moving(int ox, int oy, int ow, int oh, Orientation::Orientation dir, int movement) {
    int nx, ny, nw, nh, margin;
    switch (dir) {
        case Orientation::N: {
            nh = movement;
            nw = ow;
            ny = oy - movement;
            nx = ox;
            margin = oh / 2;
            break;
        }
        case Orientation::E: {
            nh = oh;
            nw = movement;
            ny = oy;
            nx = ox + ow;
            margin = ow / 2;
            break;
        }
        case Orientation::S: {
            nh = movement;
            nw = ow;
            ny = oy + oh;
            nx = ox;
            margin = oh / 2;
            break;
        }
        case Orientation::W: {
            nh = oh;
            nw = movement;
            ny = oy;
            nx = ox - movement;
            margin = ow / 2;
            break;
        }
    }
    vector<pair<int, objptr>> ret;
    int dist;
    for (auto& obj : objects) {
        if (obj.second->alive() && obj.second->separation_distance(nx, ny, nw, nh) < 0
            && (dist = obj.second->separation_distance(ox, oy, ow, oh)) > -margin) {
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
        f << obj.second->type() << " " << obj.second->tx() << " " << obj.second->ty() << endl;
    }
}

void ServerMap::pause() {
    is_paused_ = true;
    paused.emit();
    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::PAUSED);
    ServerMap::event({}, std::move(m));
}


void ServerMap::resume() {
    is_paused_ = false;
    resumed.emit();
    msgpackvar m;
    m["mtype"] = as_ui(ToRenderMessage::RESUMED);
    ServerMap::event({}, std::move(m));
}


std::vector<objptr> load_objects_from_file(std::istream& f, ServerMap& map) {
    int type, x, y;
    vector<objptr> ret;
    while (f >> type >> x >> y) {
        auto obj = map.add(type);
        obj->place_on_tile(x, y);
        if (type == Player::TYPE) {
            ret.push_back(obj);
        }
    }
    return ret;
}
