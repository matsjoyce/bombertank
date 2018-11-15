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
#include "../build/src/create_msg.pb.h"
#include "../build/src/generic_msg.pb.h"

using namespace std;

Map::Map(EventPipe& sep, EventPipe& rep) : es(sep, rep), object_creators(load_objects()) {
}

objptr Map::add(unsigned int type, unsigned int id) {
    return objects[id] = object_creators[type](id, this);
}

void Map::remove(objptr obj) {
    objects.erase(obj->id);
}


ServerMap::ServerMap(EventPipe& sep, EventPipe& rep) : Map(sep, rep) {
}

objptr ServerMap::add(unsigned int type) {
    auto obj = Map::add(type, next_id++);
    Message m;
    m.set_type(Message::CREATE);
    CreateMessage cm;
    cm.set_type(type);
    cm.set_id(obj->id);
    m.mutable_value()->PackFrom(cm);
    event(move(m));
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

        for (auto event : es.events()) {
            switch (event.type()) {
                case Message::PAUSE: {
                    is_paused_ = true;
                    paused.emit();
                    Message m;
                    m.set_type(Message::PAUSED);
                    Map::event(move(m));
                    break;
                }
                case Message::RESUME: {
                    is_paused_ = false;
                    resumed.emit();
                    Message m;
                    m.set_type(Message::RESUMED);
                    Map::event(move(m));
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

std::vector<objptr> ServerMap::collides(int ox, int oy, int ow, int oh) {
    vector<objptr> ret;
    for (auto& obj : objects) {
        if (!defered_destroy.count(obj.first) && obj.second->separation_distance(ox, oy, ow, oh) < 0) {
            ret.push_back(obj.second);
        }
    }
    return ret;
}

std::vector<objptr> ServerMap::collides(int ox, int oy, int ow, int oh, Orientation::Orientation dir, int movement) {
    auto dx_ = dx(dir) * movement;
    if (dx_ > 0) {
        ow += dx_;
    }
    else {
        ox += dx_;
        ow -= dx_;
    }
    auto dy_ = dy(dir) * movement;
    if (dy_ > 0) {
        oh += dy_;
    }
    else {
        oy += dy_;
        oh -= dy_;
    }
    return collides(ox, oy, ow, oh);
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
