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

#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "servermap.hpp"
#include <atomic>

enum class GMState : unsigned int;

class GameManager {
protected:
    ServerMap sm;
    std::thread thr;
    std::atomic<bool> is_running = false;
    std::map<unsigned int, std::unique_ptr<EventServer>> side_controllers;
    GMState state;
public:
    GameManager();
    virtual ~GameManager();
    ServerMap& map() {
        return sm;
    }
    void start();
    void stop();
    void add_controller(unsigned int side, std::unique_ptr<EventServer> es);
    void run();
    void broadcast_msg(msgpackvar&& m);
};

class Player;

class PVPGameManager : public GameManager {
    std::vector<Point> player_start_pos;
    std::vector<std::shared_ptr<Player>> players;
public:
    PVPGameManager();
    PVPGameManager(std::string fname);
    void add_player(Point pos);
    void player_dead();
    void player_ready();
    unsigned int sides() {
        return players.size();
    }
    bool done();
};

class EditorGameManager : public GameManager {
public:
    EditorGameManager(std::string fname);
};

#endif // GAMEMANAGER_HPP
