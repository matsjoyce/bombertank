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

#include "gamemanager.hpp"
#include "objects/player.hpp"
#include <fstream>
#include <iostream>

using namespace std;

GameManager::GameManager(std::string fname) {
    sm.set_event_sender(std::bind(&GameManager::broadcast_msg, this, placeholders::_1));
}

void GameManager::broadcast_msg(msgpackvar && m) {
    for (auto& sc : side_controllers) {
        auto mc = m;
        sc.second->send(std::move(mc));
    }
}


void GameManager::add_controller(unsigned int side, std::unique_ptr<EventServer> es) {
    side_controllers.emplace(make_pair(side, move(es)));
}

void GameManager::start() {
    thr = thread([this]{run();});
}

void GameManager::stop() {
    is_running = false;
    thr.join();
}

GameManager::~GameManager() {
    if (thr.joinable()) {
        stop();
    }
}

void GameManager::run() {
    cout << "GameManager::run is running!" << endl;
    is_running = true;
    while (is_running) {
        auto start = chrono::high_resolution_clock::now();
        for (auto& sc : side_controllers) {
            for (auto event : sc.second->events()) {
                switch (static_cast<ToServerMessage>(event["mtype"].as_uint64_t())) {
                    case ToServerMessage::PAUSE: {
                        sm.pause("Paused");
                        break;
                    }
                    case ToServerMessage::RESUME: {
                        sm.resume();
                        break;
                    }
                    default: {
                        sm.handle(std::move(event));
                    }
                }
            }
        }
        sm.update();
        auto end = chrono::high_resolution_clock::now();
        this_thread::sleep_for(std::chrono::milliseconds(50) - (end - start));
    }
}

std::vector<Point> load_objects_from_file(std::istream& f, ServerMap& map) {
    int type, x, y;
    vector<Point> ret;
    while (f >> type >> x >> y) {
        if (type == Player::TYPE) {
            ret.push_back(Point(x, y).from_tile());
        }
        else {
            auto obj = map.add(type);
            obj->set_nw_corner(Point(x, y).from_tile());
            obj->_generate_move();
        }
    }
    return ret;
}

enum PVPState : unsigned int {
    PLAYING,
    WAITING_FOR_PLAYER,
    GAME_OVER
};

PVPGameManager::PVPGameManager(std::string fname) : GameManager(fname) {
    sm.pause("Starting");
    auto f = ifstream(fname);
    player_start_pos = load_objects_from_file(f, sm);
    int i = 0;
    for (auto& pos : player_start_pos) {
        auto player = dynamic_pointer_cast<Player>(sm.add(Player::TYPE));
        players.emplace_back(player);
        player->set_nw_corner(pos);
        player->_generate_move();
        player->set_side(i);
        player->destroyed.connect([this]{player_dead();});
        ++i;
    };
}

void PVPGameManager::player_dead() {
    for (auto i = 0u; i != players.size(); ++i) {
        auto player = players[i];
        if (!player->alive()) {
            if (!player->lives()) {
                state = GAME_OVER;
                sm.pause("Game Over!");
                return;
            }
            auto new_player = players[i] = dynamic_pointer_cast<Player>(sm.add(Player::TYPE));
            player->transfer(new_player);
            new_player->set_nw_corner(player_start_pos[i]);
            new_player->_generate_move();
            new_player->set_side(i);
            new_player->destroyed.connect([this]{player_dead();});
        }
    }
    state = WAITING_FOR_PLAYER;
    sm.pause("Someone Died!");
}

bool PVPGameManager::done() {
    return state == GAME_OVER;
}
