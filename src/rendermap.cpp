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
#include <variant>
#include <iostream>

using namespace std;

RenderMap::RenderMap(std::unique_ptr<EventServer> evs, unsigned int side) : es(move(evs)), side_(side) {
}

void RenderMap::event(msgpackvar&& msg) {
    es->send(std::move(msg));
}

void RenderMap::update() {
    for (auto event : es->events()) {
        switch (static_cast<ToRenderMessage>(event["mtype"].as_uint64_t())) {
            case ToRenderMessage::CREATE: {
                auto obj = add(event["type"].as_uint64_t(), event["id"].as_uint64_t());
                obj->set_nw_corner(extract_int(event["x"]), extract_int(event["y"]));
                obj->_generate_move();
                obj->set_side(event["side"].as_uint64_t());
                layers.insert(make_pair(obj->render_layer(), obj));
                break;
            }
            case ToRenderMessage::PAUSED: {
                is_paused_ = true;
                pause_reason = event["reason"].as_string();
                pause_darkness = 0;
                paused.emit();
                break;
            }
            case ToRenderMessage::RESUMED: {
                is_paused_ = false;
                resumed.emit();
                break;
            }
            case ToRenderMessage::FOROBJ: {
                auto id = event["id"].as_uint64_t();
                if (objects.count(id) && objects[id]) {
                    objects[id]->render_handle(event);
                }
                else {
                    msgpack::zone z;
                    cout << "Event for non-existent object " << id << " " << msgpack::object(event, z) << ", have " << objects.size() << " objects" << endl;
                }
                break;
            }
            default: {
                cout << "Unhandled event in RenderMap: " << event["type"].as_uint64_t() << endl;
            }
        }
    }
    for (auto iter = objects.begin(); iter != objects.end();) {
        if (iter->second) {
            iter->second->render_update();
            ++iter;
        }
        else {
            objects.erase(iter++);
        }
    }
    for (auto iter = effects.begin(); iter != effects.end();) {
        if (iter->second) {
            iter->second->update();
            ++iter;
        }
        else {
            effects.erase(iter++);
        }
    }
}

void RenderMap::render(sf::RenderTarget& rt) {
    auto view = rt.getView();
    view.setCenter(center());
    rt.setView(view);
    for (auto& obj : layers) {
        if (obj.second.index() == 0) {
            get<0>(obj.second)->render(rt);
        }
        else {
            get<1>(obj.second)->render(rt);
        }
    }
    view.reset(sf::FloatRect({}, view.getSize()));
    if (is_paused()) {
        auto sc_view = view;
        sc_view.reset(sf::FloatRect({}, view.getSize() / 6.0f));
        rt.setView(sc_view);
        sf::RectangleShape drect(sf::Vector2f(sc_view.getSize()));
        drect.setFillColor(sf::Color(0, 0, 0, pause_darkness));
        rt.draw(drect);

        sf::Text text(pause_reason, load_font("data/fonts/font.pcf"), 12);

        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,
                       textRect.top  + textRect.height/2.0f);
        text.setPosition(sc_view.getSize() / 2.0f);
        text.setFillColor(sf::Color(255, 0, 0, pause_darkness));
        rt.draw(text);
        if (pause_darkness < 200) {
            ++pause_darkness;
        }
    }
    if (following) {
        rt.setView(view);
        following->render_hud(rt);
    }
}

void RenderMap::register_keypress(sf::Keyboard::Key key, unsigned int id) {
    keypress_register[key] = id;
}

void RenderMap::handle_keypress(sf::Keyboard::Key key, bool is_down) {
    if (keypress_register.count(key) && objects.count(keypress_register[key]) && objects[keypress_register[key]]) {
        objects[keypress_register[key]]->handle_keypress(key, is_down);
    }
    else {
        cout << "Unhandled keypress " << key << endl;
    }
}

const sf::Texture& RenderMap::load_texture(std::string path) {
    auto& tex = textures[path];
    if (!tex.getNativeHandle()) {
        cout << "Loading " << path << " as texture" << endl;
        if (!tex.loadFromFile(path)) {
            cout << "Could not load " << path << endl;
        }
    }
    return tex;
}

const sf::SoundBuffer& RenderMap::load_sound_buf(std::string path) {
    auto& sndbuf = sound_bufs[path];
    if (!sndbuf.getSampleCount()) {
        cout << "Loading " << path << " as soundbuf" << endl;
        if (!sndbuf.loadFromFile(path)) {
            cout << "Could not load " << path << endl;
        }
    }
    return sndbuf;
}

const sf::Font& RenderMap::load_font(std::string path) {
    auto font_iter = fonts.find(path);
    if (font_iter != fonts.end()) {
        return font_iter->second;
    }
    auto& font = fonts[path];
    cout << "Loading " << path << " as font" << endl;
    if (!font.loadFromFile(path)) {
        cout << "Could not load " << path << endl;
    }
    // HACK! If we load a font that uses a different size we have a problem.
    const_cast<sf::Texture&>(font.getTexture(12)).setSmooth(false);
    return font;
}

void RenderMap::remove_effect(unsigned int id) {
    for (auto iter = layers.lower_bound(effects[id]->layer()); iter != layers.end(); ++iter) {
        if (iter->second.index() == 1 && get<1>(iter->second)->id == id) {
            layers.erase(iter);
            break;
        }
    }
    effects[id] = {};
}

void RenderMap::remove(objptr obj) {
    for (auto iter = layers.lower_bound(obj->render_layer()); iter != layers.end(); ++iter) {
        if (iter->second.index() == 0 && get<0>(iter->second)->id == obj->id) {
            layers.erase(iter);
            break;
        }
    }
    objects[obj->id] = {};
}

void RenderMap::pause() {
    msgpackvar m;
    m["mtype"] = as_ui(ToServerMessage::PAUSE);
    event(std::move(m));
}

void RenderMap::resume() {
    msgpackvar m;
    m["mtype"] = as_ui(ToServerMessage::RESUME);
    event(std::move(m));
}

void RenderMap::follow(objptr obj) {
    following = obj;
}

void RenderMap::center_on(Point center) {
    center_ = center;
}

Point RenderMap::center() {
    if (following) {
        return following->center();
    }
    return center_;
}

