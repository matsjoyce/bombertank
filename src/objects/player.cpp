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

#include "player.hpp"
#include "../../build/src/generic_msg.pb.h"
#include "bomb.hpp"
#include "walls.hpp"

using namespace std;

enum PlayerCommands {
    START_MOVE, END_MOVE, DROP_BOMB, DROP_WALL
};

struct PlayerSettings {
    sf::Keyboard::Key up, down, left, right, bomb, wall;
    sf::Color color;
};

map<int, PlayerSettings> player_settings = {
    {0, {sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::B, sf::Keyboard::G, sf::Color(128, 0, 0)}},
    {1, {sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Enter, sf::Keyboard::RBracket, sf::Color(0, 128, 0)}},
};

Player::Player(unsigned int id_, Map* map_) : Object(id_, map_) {
    if (render_map()) {
        destroyed.connect([this]{
            render_map()->add_effect<DeadPlayer>(x(), y(), orientation());
        });
        setup_keys();
        side_changed.connect([this]{setup_keys();});
    }
    auto fid = map->paused.connect([this]{
        direction_stack.clear();
    });
    destroyed.connect([this, fid]{map->paused.disconnect(fid);});
}

void Player::render(sf::RenderTarget& rt) {
    if (speed()) {
        auto t = fmod(anim_clock.getElapsedTime().asSeconds() * 24, 4);
        tex_name = t < 1 ? "data/images/tank1.png" : t < 2 ? "data/images/tank2.png" : t < 3 ? "data/images/tank3.png" : "data/images/tank4.png";
    }
    sf::Sprite sp(render_map()->load_texture(tex_name));
    sp.setOrigin(sf::Vector2f(width / 2, height / 2));
    sp.setPosition(sf::Vector2f(x() + width / 2, y() + height / 2));
    sp.setRotation(angle(orientation()));
    rt.draw(sp);

    sf::Sprite sp2(render_map()->load_texture("data/images/tank_colored.png"));
    sp2.setOrigin(sf::Vector2f(width / 2, height / 2));
    sp2.setPosition(sf::Vector2f(x() + width / 2, y() + height / 2));
    sp2.setRotation(angle(orientation()));
    sp2.setColor(player_settings[side()].color);
    rt.draw(sp2);
}

void Player::handle_keypress(sf::Keyboard::Key key, bool is_down) {
    Message m;
    m.set_id(id);
    m.set_type(Message::PLAYER_COMMAND);
    GenericMessage gm;
    if (key == player_settings[side()].bomb) {
        if (!is_down) {
            return;
        }
        gm.set_id(DROP_BOMB);
    }
    else if (key == player_settings[side()].wall) {
        if (!is_down) {
            return;
        }
        gm.set_id(DROP_WALL);
    }
    else {
        gm.set_id(is_down ? START_MOVE : END_MOVE);
        cout << key << " " << is_down << " " << gm.id() << endl;
        if (key == player_settings[side()].left) gm.set_orientation(Orientation::W);
        else if (key == player_settings[side()].right) gm.set_orientation(Orientation::E);
        else if (key == player_settings[side()].up) gm.set_orientation(Orientation::N);
        else if (key == player_settings[side()].down) gm.set_orientation(Orientation::S);
    }
    m.mutable_value()->PackFrom(gm);
    render_map()->event(move(m));
}

void Player::handle(Message m) {
    switch (m.type()) {
        case Message::PLAYER_COMMAND: {
            GenericMessage gm;
            m.value().UnpackTo(&gm);
            switch (gm.id()) {
                case START_MOVE: {
                    direction_stack.push_back(static_cast<Orientation::Orientation>(gm.orientation()));
                    break;
                }
                case END_MOVE: {
                    auto pos = find(direction_stack.begin(), direction_stack.end(), static_cast<Orientation::Orientation>(gm.orientation()));
                    if (pos == direction_stack.end()) {
                        cout << "No move to end " << gm.orientation() << endl;
                    }
                    else {
                        direction_stack.erase(pos);
                    }
                    break;
                }
                case DROP_BOMB: {
                    cout << "DROP_BOMB " << num_bombs << endl;
                    if (num_bombs) {
                        --num_bombs;
                        auto obj = server_map()->add(TimedBomb::TYPE);
                        obj->place(tcx(), tcy());
                        obj->destroyed.connect([this]{++num_bombs;});
                    }
                    break;
                }
                case DROP_WALL: {
                    if (num_walls) {
                        --num_walls;
                        auto obj = server_map()->add(PlacedWall::TYPE);
                        obj->place(tcx(), tcy());
                        obj->destroyed.connect([this]{++num_walls;});
                    }
                    break;
                }
                default: {
                    cout << "Unhandled PC " << gm.id() << " in Player::handle" << endl;
                }
            }
            break;
        }
        default: Object::handle(m);
    }
}

void Player::setup_keys() {
    if (side() != static_cast<unsigned int>(-1)) {
        if (auto rmap = render_map()) {
            if (rmap->side() == side()) {
                rmap->follow(shared_from_this());
                rmap->register_keypress(player_settings[side()].left, id);
                rmap->register_keypress(player_settings[side()].right, id);
                rmap->register_keypress(player_settings[side()].up, id);
                rmap->register_keypress(player_settings[side()].down, id);
                rmap->register_keypress(player_settings[side()].bomb, id);
                rmap->register_keypress(player_settings[side()].wall, id);
            }
        }
    }
}

void Player::render_handle(Message m) {
    switch (m.type()) {
        case Message::PLAYER_COMMAND: {
            GenericMessage gm;
            m.value().UnpackTo(&gm);
            switch (gm.id()) {
                default: {
                    cout << "Unhandled PC " << gm.id() << " in Player::render_handle" << endl;
                }
            }
            break;
        }
        default: Object::render_handle(m);
    }
}


void Player::update() {
    if (direction_stack.size()) {
        if (direction() != direction_stack.back()) {
            set_direction(direction_stack.back());
            set_orientation(direction_stack.back());
        }
        accelerate(max_speed - speed());
    }
}

unsigned int Player::max_hp() {
    return 150;
}

unsigned int Player::layer() {
    return 6;
}

void DeadPlayer::render(sf::RenderTarget& rt) {
    sf::Sprite sp(map->load_texture("data/images/tank_dead.png"));
    sp.setOrigin(sf::Vector2f(STANDARD_OBJECT_SIZE / 2, STANDARD_OBJECT_SIZE / 2));
    sp.setPosition(sf::Vector2f(x + STANDARD_OBJECT_SIZE / 2, y + STANDARD_OBJECT_SIZE / 2));
    sp.setRotation(angle(orientation));
    rt.draw(sp);
}

unsigned int DeadPlayer::layer() {
    return 1;
}
