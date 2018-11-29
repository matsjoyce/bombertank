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

#ifndef MAP_HPP
#define MAP_HPP

#include <map>
#include <memory>
#include <set>
#include "eventqueue.hpp"
#include "object.hpp"
#include "signal.hpp"

enum class ToRenderMessage : unsigned int {
    CREATE,
    FOROBJ,
    PAUSED,
    RESUMED
};

enum class ToServerMessage : unsigned int {
    PAUSE,
    RESUME,
    FOROBJ
};

class Map {
protected:
    Map();
    virtual ~Map() = default;
    std::map<unsigned int, std::function<objptr(unsigned int, Map*)>> object_creators;
    std::map<unsigned int, objptr> objects;
    objptr add(unsigned int type, unsigned int id);
    virtual void remove(objptr obj);
    bool is_paused_ = false;
public:
    Signal<> paused;
    Signal<> resumed;
    inline bool is_paused() const {
        return is_paused_;
    }
    friend class Object;
};

class ServerMap : public Map {
    unsigned int next_id = 1;
    void remove(objptr obj) override;
    std::set<unsigned int> defered_destroy;
    bool in_update = false;
    bool is_running = false;
    std::map<unsigned int, std::unique_ptr<EventServer>> side_controllers;
    std::deque<std::pair<objptr, msgpackvar>> pending_events;
    std::recursive_mutex mutex;
    bool events_paused = false;
public:
    void add_controller(unsigned int side, std::unique_ptr<EventServer> es);
    void update();
    void run();
    void halt();
    void pause(bool events_too=false);
    void resume();
    void event(objptr obj, msgpackvar&& msg);
    objptr add(unsigned int type);
    std::vector<std::pair<int, objptr>> collides(int ox, int oy, unsigned int ow, unsigned int oh);
    std::vector<std::pair<int, objptr>> collides_by_moving(int ox, int oy, unsigned int ow, unsigned int oh, Orientation::Orientation dir, int movement, bool skip_start=true);
    void save_objects_to_map(std::ostream& f);
};

std::vector<objptr> load_objects_from_file(std::istream& f, ServerMap& map);

#endif // MAP_HPP
