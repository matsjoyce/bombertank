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

#include "rendermap.hpp"
#include "../build/src/generic_msg.pb.h"
#include <variant>

using namespace std;

RenderMap::RenderMap(std::unique_ptr<EventServer> evs, unsigned int side) : es(move(evs)), side_(side) {
}

void RenderMap::event(Message&& msg) {
    es->send(move(msg));
}

void RenderMap::update() {
    for (auto event : es->events()) {
        switch (event.type()) {
            case Message::CREATE: {
                if (!event.value().Is<GenericMessage>()) {
                    throw runtime_error("Create message's value is not a GenericMessage");
                }
                GenericMessage gm;
                event.value().UnpackTo(&gm);
                auto obj = add(gm.type(), gm.id());
                obj->place(gm.x(), gm.y());
                obj->set_side(gm.side());
                layers.insert(make_pair(obj->layer(), obj));
                break;
            }
            case Message::PAUSED: {
                is_paused_ = true;
                paused.emit();
                break;
            }
            case Message::RESUMED: {
                is_paused_ = false;
                resumed.emit();
                break;
            }
            default: {
                if (event.id()) {
                    if (objects.count(event.id())) {
                        objects[event.id()]->render_handle(event);
                    }
                    else {
                        cout << "Event " << event.type() << " for non-existent object " << event.id() << endl;
                    }
                }
                else {
                    cout << "Unhandled event in RenderMap: " << event.type() << endl;
                }
            }
        }
    }
    for (auto& obj : objects) {
        obj.second->render_update();
    }
    for (auto& eff : effects) {
        eff.second->update();
    }
}

void RenderMap::render(sf::RenderTarget& rt) {
    auto view = rt.getView();
    if (following) {
        view.setCenter(sf::Vector2f(following->x(), following->y()));
    }
    rt.setView(view);
    for (auto& obj : layers) {
        if (obj.second.index() == 0) {
            get<0>(obj.second)->render(rt);
        }
        else {
            get<1>(obj.second)->render(rt);
        }
    }
    if (following) {
        view.reset(sf::FloatRect({}, view.getSize()));
        rt.setView(view);

        sf::RectangleShape border(sf::Vector2f(202, 8));
        border.setPosition(1, 1);
        border.setFillColor(sf::Color::Black);
        rt.draw(border);

        auto texbg = load_texture("data/images/hp_bar_bg.png");
        texbg.setRepeated(true);
        sf::Sprite sp_bg(texbg);
        sp_bg.setTextureRect(sf::IntRect(0, 0, 200, 6));
        sp_bg.setPosition(2, 2);
        rt.draw(sp_bg);

        auto texfg = load_texture("data/images/hp_bar_fg.png");
        texfg.setRepeated(true);
        sf::Sprite sp_fg(texfg);
        cout << following->hp() << " " << following->max_hp() << endl;
        sp_fg.setTextureRect(sf::IntRect(0, 0, min(200u, 200 * following->hp() / following->max_hp()), 6));
        sp_fg.setPosition(2, 2);
        rt.draw(sp_fg);
    }
}

void RenderMap::register_keypress(sf::Keyboard::Key key, unsigned int id) {
    keypress_register[key] = id;
}

void RenderMap::handle_keypress(sf::Keyboard::Key key, bool is_down) {
    if (!is_paused() && keypress_register.count(key) && objects.count(keypress_register[key])) {
        objects[keypress_register[key]]->handle_keypress(key, is_down);
    }
    else {
        cout << "Unhandled keypress " << key << endl;
    }
}

const sf::Texture& RenderMap::load_texture(std::string path) {
    auto& tex = textures[path];
    if (!tex.getNativeHandle()) {
        cout << "Making" << endl;
        if (!tex.loadFromFile(path)) {
            cout << "Could not load " << path << endl;
        }
    }
    return tex;
}

const sf::SoundBuffer& RenderMap::load_sound_buf(std::string path) {
    auto& sndbuf = sound_bufs[path];
    if (!sndbuf.getSampleCount()) {
        cout << "Making" << endl;
        if (!sndbuf.loadFromFile(path)) {
            cout << "Could not load " << path << endl;
        }
    }
    return sndbuf;
}

void RenderMap::remove_effect(unsigned int id) {
    for (auto iter = layers.lower_bound(effects[id]->layer()); iter != layers.end(); ++iter) {
        if (iter->second.index() == 1 && get<1>(iter->second)->id == id) {
            layers.erase(iter);
            break;
        }
    }
    effects.erase(id);
}

void RenderMap::remove(objptr obj) {
    for (auto iter = layers.lower_bound(obj->layer()); iter != layers.end(); ++iter) {
        if (iter->second.index() == 0 && get<0>(iter->second)->id == obj->id) {
            layers.erase(iter);
            break;
        }
    }
    Map::remove(obj);
}

void RenderMap::pause() {
    Message m;
    m.set_type(Message::PAUSE);
    event(move(m));
}

void RenderMap::resume() {
    Message m;
    m.set_type(Message::RESUME);
    event(move(m));
}

void RenderMap::follow(objptr obj) {
    following = obj;
}
