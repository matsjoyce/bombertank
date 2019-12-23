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

#ifndef SERVERMAP_HPP
#define SERVERMAP_HPP

#include "map.hpp"

class ServerMap : public Map {
    unsigned int next_id = 1;
    std::set<unsigned int> defered_destroy;
    bool in_update = false;
    std::deque<std::pair<objptr, msgpackvar>> pending_events;
    std::recursive_mutex mutex;
    bool events_paused = false;
    unsigned int level_ups_created = 0;
    unsigned int frame_ = 0;
    std::multimap<unsigned int, std::function<void()>> frame_callbacks;
    std::function<void(msgpackvar&& m)> ev_sender;
public:
    const unsigned int frame_rate = 20;
    void update(bool update_objs=true);
    void pause(std::string reason, bool events_too=false);
    void resume();
    void event(objptr obj, msgpackvar&& msg);
    void remove(objptr obj) override;
    objptr add(unsigned int type);
    std::vector<std::pair<int, objptr>> collides(const Rect& r, unsigned int layer=-1);
    std::vector<std::pair<int, objptr>> collides(const Rect& r, std::function<int(objptr)> sortfunc, unsigned int layer=-1);
    std::vector<std::pair<int, objptr>> collides_by_moving(const Rect& r, Orientation::Orientation dir, int movement, unsigned int layer=-1);
    void save_objects_to_map(std::ostream& f);
    void level_up_trigger(objptr obj);
    unsigned int frame() const {
        return frame_;
    }
    void on_frame(unsigned int frame, std::function<void()> f);
    void set_event_sender(std::function<void(msgpackvar&& m)> f) {
        ev_sender = f;
    } // HACK?
    void handle(msgpackvar&& m);
};

#endif // SERVERMAP_HPP
