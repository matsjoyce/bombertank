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

#ifndef RENDERMAP_HPP
#define RENDERMAP_HPP

#include "map.hpp"
#include <variant>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "effect.hpp"
#include "object.hpp"


class RenderMap : public Map {
    std::map<sf::Keyboard::Key, unsigned int> keypress_register;
    std::map<unsigned int, effptr> effects;
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::SoundBuffer> sound_bufs;
    std::map<std::string, sf::Font> fonts;
    std::multimap<unsigned int, std::variant<objptr, effptr>> layers;
    unsigned int next_effect = 1;
    void remove(objptr obj) override;
    std::unique_ptr<EventServer> es;
    unsigned int side_;
    objptr following;
    Point center_;
    unsigned int pause_darkness = 0;
    std::string pause_reason;
public:
    RenderMap(std::unique_ptr<EventServer> evs, unsigned int side);
    void update();
    void render(sf::RenderTarget& rt);
    void render_editor_tile(sf::RenderTarget& rt, sf::Vector2f position, unsigned int id);
    void register_keypress(sf::Keyboard::Key key, unsigned int id);
    void handle_keypress(sf::Keyboard::Key key, bool is_down);
    const sf::Texture& load_texture(std::string path);
    const sf::SoundBuffer& load_sound_buf(std::string path);
    const sf::Font& load_font(std::string path);
    template<class T, class... A> effptr add_effect(A... args) {
        auto obj = std::make_shared<T>(this, next_effect++, args...);
        effects[obj->id] = obj;
        layers.insert(make_pair(obj->layer(), obj));
        return obj;
    }
    void remove_effect(unsigned int id);
    void pause();
    void resume();
    void event(msgpackvar&& msg);
    unsigned int side() const {
        return side_;
    }
    void follow(objptr obj);
    void center_on(Point center);
    Point center();
    std::vector<unsigned int> editor_tiles();
};

#endif // RENDERMAP_HPP
