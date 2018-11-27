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
#include "../build/src/generic_msg.pb.h"

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
    for (auto& obj : objects) {
        Message m;
        m.set_type(Message::CREATE);
        GenericMessage gm;
        gm.set_type(obj.second->type());
        gm.set_id(obj.second->id);
        gm.set_x(obj.second->x());
        gm.set_y(obj.second->y());
        gm.set_side(obj.second->side());
        m.mutable_value()->PackFrom(gm);
        side_controllers[side]->send(move(m));
    }
}

void ServerMap::event(objptr obj, Message&& msg) {
    for (auto& sc : side_controllers) {
        auto m = msg;
        sc.second->send(move(m));
    }
}

objptr ServerMap::add(unsigned int type) {
    auto obj = Map::add(type, next_id++);
    Message m;
    m.set_type(Message::CREATE);
    GenericMessage gm;
    gm.set_type(type);
    gm.set_id(obj->id);
    gm.set_x(obj->x());
    gm.set_y(obj->y());
    gm.set_side(obj->side());
    m.mutable_value()->PackFrom(gm);
    event(obj, move(m));
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
}

void ServerMap::run() {
    is_running = true;
    while (is_running) {
        auto start = chrono::high_resolution_clock::now();

        for (auto& sc : side_controllers) {
            for (auto event : sc.second->events()) {
                switch (event.type()) {
                    case Message::PAUSE: {
                        pause();
                        break;
                    }
                    case Message::RESUME: {
                        resume();
                        break;
                    }
                    default: {
                        if (event.id()) {
                            if (is_paused_) {
                                cout << "Event " << event.type() << " for obj " << event.id() << " while paused!" << endl;
                            }
                            else if (objects.count(event.id())) {
                                objects[event.id()]->handle(event);
                            }
                            else {
                                cout << "Event " << event.type() << " for non-existent object " << event.id() << endl;
                            }
                        }
                        else {
                            cout << "Unhandled event in ServerMap: " << event.type() << endl;
                        }
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


vector<objptr> ServerMap::objs_at_dir(objptr obj, Orientation::Orientation dir) {
    vector<objptr> ret;
    for (auto& obj2 : objects) {
        if (obj->id != obj2.first && Orientation::angle_diff(dir, obj->angle_to(obj2.second)) < 45) {
            ret.push_back(obj2.second);
        }
    }
    return ret;
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
    int nx, ny, nw, nh;
    switch (dir) {
        case Orientation::N: {
            nh = movement;
            nw = ow;
            ny = oy - movement;
            nx = ox;
            break;
        }
        case Orientation::E: {
            nh = oh;
            nw = movement;
            ny = oy;
            nx = ox + ow;
            break;
        }
        case Orientation::S: {
            nh = movement;
            nw = ow;
            ny = oy + oh;
            nx = ox;
            break;
        }
        case Orientation::W: {
            nh = oh;
            nw = movement;
            ny = oy;
            nx = ox - movement;
            break;
        }
    }
    vector<pair<int, objptr>> ret;
    int dist;
    for (auto& obj : objects) {
        if (obj.second->alive() && obj.second->separation_distance(nx, ny, nw, nh) < 0
            && (dist = obj.second->separation_distance(ox, oy, ow, oh)) > -static_cast<int>(obj.second->width) / 2) {
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
    Message m;
    m.set_type(Message::PAUSED);
    ServerMap::event({}, move(m));
}


void ServerMap::resume() {
    is_paused_ = false;
    resumed.emit();
    Message m;
    m.set_type(Message::RESUMED);
    ServerMap::event({}, move(m));
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
