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

#include "stagemanager.hpp"
#include "servermap.hpp"
#include "rendermap.hpp"
#include "gamemanager.hpp"
#include "objects/player.hpp"
#include <fstream>
#include <thread>
#include <filesystem>
#include <list>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

constexpr unsigned int SCALEUP = 2;

// TODO WARNING: The below code interacts with the ServerMap directly which is NOT threadsafe! Must be migrated to use rendermaps!

struct GameState {
    unique_ptr<GameManager> gm;
    vector<Point> starting_positions;
    vector<shared_ptr<Player>> players;
    vector<RenderMap> rms;
    sf::Font font;
    int dpi_scaling_factor = 1;
    GameState() {
        font.loadFromFile("data/fonts/font.pcf");
        const_cast<sf::Texture&>(font.getTexture(12)).setSmooth(false);
    }
};

unique_ptr<Stage> Stage::update(sf::RenderWindow& /*window*/) {
    return {};
}

void Stage::render(sf::RenderWindow& /*window*/) {
}

LoadStage::LoadStage(bool is_editor) : gstate(make_unique<GameState>()), load_editor(is_editor) {
}

vector<string> LoadStage::load_maps(std::string dir) {
    vector<string> maps;
    for (auto& item : fs::directory_iterator(dir)) {
        if (item.is_directory()) {
            auto subdir = load_maps(item.path());
            maps.insert(maps.end(), subdir.begin(), subdir.end());
        }
        else if (item.is_regular_file() && item.path().extension() == ".btm") {
            maps.push_back(item.path());
        }
    }
    sort(maps.begin(), maps.end());
    return maps;
}

unique_ptr<Stage> LoadStage::update(sf::RenderWindow& window) {
    if (window.getSize().x > 2000) {
        gstate->dpi_scaling_factor = 2;
    }
    if (load_editor) {
        auto maps = load_maps("maps");
        maps.insert(maps.begin(), "<New>");
        auto ss = make_unique<SelectStage>(move(gstate), maps);

        ss->on_selected = [](unsigned int index, string value, unique_ptr<GameState>&& gstate){
            gstate->gm = make_unique<EditorGameManager>(index ? value : "map_save.btm");

            auto es1 = make_unique<EventServer>(), es2 = make_unique<EventServer>();
            es1->connect(es2.get());
            es2->connect(es1.get());
            auto& rm = gstate->rms.emplace_back(move(es1), 0);
            rm.set_is_editor(true);
            gstate->gm->add_controller(0, move(es2));

            return make_unique<EditorStage>(move(gstate), index ? value : "map_save.btm");
        };
        return move(ss);
    }
    else {
        auto ss = make_unique<SelectStage>(move(gstate), load_maps("maps"));

        ss->on_selected = [](unsigned int /*index*/, string value, unique_ptr<GameState>&& gstate){
            gstate->gm = make_unique<PVPGameManager>(value);
            auto gm = dynamic_cast<PVPGameManager*>(gstate->gm.get());
            for (auto i = 0u; i < gm->sides(); ++i) {
                auto es1 = make_unique<EventServer>(), es2 = make_unique<EventServer>();
                es1->connect(es2.get());
                es2->connect(es1.get());
                gstate->rms.emplace_back(move(es1), i);
                gm->add_controller(i, move(es2));
            };
            gm->start();
            return make_unique<PlayStage>(move(gstate));
        };
        return move(ss);
    }
}

std::unique_ptr<Stage> LoadStage::create(bool is_editor/*=false*/) {
    return make_unique<LoadStage>(is_editor);
}

SelectStage::SelectStage(std::unique_ptr<GameState> gs, std::vector<std::string> vals) : gstate(move(gs)), values(vals) {
}

unique_ptr<Stage> SelectStage::update(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Enter) {
                return on_selected(current_index, values[current_index], move(gstate));
            }
            else if (event.key.code == sf::Keyboard::Down) {
                current_index = (current_index + 1) % values.size();
            }
            else if (event.key.code == sf::Keyboard::Up) {
                current_index = (current_index - 1 + values.size()) % values.size();
            }
        }
    }
    return {};
}

void SelectStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect(sf::Vector2f(0, 0), Point(window.getSize()) / gstate->dpi_scaling_factor / SCALEUP));
    view.setViewport(sf::FloatRect(0, 0, 1.0, 1.0));
    window.setView(view);

    int y = 0;
    for (auto& value : values) {
        sf::Text txt(value, gstate->font, 12);
        txt.setPosition(sf::Vector2f(6, 6 + 14 * y));
        txt.setFillColor(y == current_index ? sf::Color::Red : sf::Color::Black);
        window.draw(txt);
        ++y;
    }
}

PlayStage::PlayStage(std::unique_ptr<GameState> gs) : gstate(move(gs)) {
}

unique_ptr<Stage> PlayStage::update(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                cout << "PAUSE/RESUME" << endl;
                if (!gstate->rms[0].is_paused()) {
                    gstate->rms[0].pause();
                }
                else if (dynamic_cast<PVPGameManager*>(gstate->gm.get())->done()) {
                    return make_unique<LoadStage>();
                }
                else {
                    gstate->rms[0].resume();
                }
            }
            else if (event.key.control && event.key.code == sf::Keyboard::Q) {
                return make_unique<LoadStage>();
            }
            else {
                for (auto& rm : gstate->rms) {
                    rm.handle_keypress(event.key.code, true);
                }
            }
        }
        if (event.type == sf::Event::KeyReleased) {
            for (auto& rm : gstate->rms) {
                rm.handle_keypress(event.key.code, false);
            }
        }
    }
    for (auto& rm : gstate->rms) {
        rm.update();
    }
    return {};
}

void PlayStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect(0, 0, window.getSize().x / SCALEUP / gstate->dpi_scaling_factor, window.getSize().y / SCALEUP / 2 / gstate->dpi_scaling_factor));

    if (gstate->rms.size()) {
        view.setViewport(sf::FloatRect(0, 0, 1.0, 0.5));
        window.setView(view);
        gstate->rms[0].render(window);
    }

    if (gstate->rms.size() > 1) {
        view.setViewport(sf::FloatRect(0, 0.5, 1.0, 0.5));
        window.setView(view);
        gstate->rms[1].render(window);
    }
}

EditorStage::EditorStage(std::unique_ptr<GameState> gs, std::string fname) : gstate(move(gs)), fname_(fname) {
}

unique_ptr<Stage> EditorStage::update(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            auto pos = ((Point(event.mouseButton.x, event.mouseButton.y) - Point(window.getSize()) / 2) / SCALEUP / gstate->dpi_scaling_factor).to_tile() + gstate->rms[0].center().to_tile();
            auto r = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ? Rect(pos, last_pos) : Rect(pos, pos);
            r.set_size(r.width() + 1, r.height() + 1);
            Rect collides_rect(STANDARD_OBJECT_SIZE, STANDARD_OBJECT_SIZE);
            for (auto point : RectangularIterator(r)) {
                collides_rect.set_nw_corner(point.from_tile());
                cout << gstate->gm->map().collides(collides_rect).size() << endl;
                if (!gstate->gm->map().collides(collides_rect).size()) {
                    auto obj = gstate->gm->map().add(placing);
                    obj->set_nw_corner(point.from_tile());
                    obj->_generate_move();
                }
            }
            last_pos = pos;
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.control && event.key.code == sf::Keyboard::S) {
                cout << "Saving to " << fname_ << endl;
                ofstream f(fname_);
                gstate->gm->map().save_objects_to_map(f);
                f.close();
            }
            else if (event.key.control && event.key.code == sf::Keyboard::Q) {
                return make_unique<LoadStage>(true);
            }
            else if (event.key.code == sf::Keyboard::Delete) {
                auto pos = ((Point(sf::Mouse::getPosition(window)) - Point(window.getSize()) / 2) / SCALEUP / gstate->dpi_scaling_factor).to_tile() + gstate->rms[0].center().to_tile();
                auto r = event.key.shift ? Rect(pos.from_tile(), last_pos.from_tile()) : Rect(pos.from_tile(), pos.from_tile());
                r.set_size(r.width() + STANDARD_OBJECT_SIZE, r.height() + STANDARD_OBJECT_SIZE);
                for (auto& obj : gstate->gm->map().collides(r)) {
                    obj.second->destroy();
                }
                last_pos = pos;
            }
            else if (event.key.code == sf::Keyboard::W) {
                gstate->rms[0].center_on(gstate->rms[0].center() - Point(0, STANDARD_OBJECT_SIZE));
            }
            else if (event.key.code == sf::Keyboard::A) {
                gstate->rms[0].center_on(gstate->rms[0].center() - Point(STANDARD_OBJECT_SIZE, 0));
            }
            else if (event.key.code == sf::Keyboard::S) {
                gstate->rms[0].center_on(gstate->rms[0].center() + Point(0, STANDARD_OBJECT_SIZE));
            }
            else if (event.key.code == sf::Keyboard::D) {
                gstate->rms[0].center_on(gstate->rms[0].center() + Point(STANDARD_OBJECT_SIZE, 0));
            }
        }
        if (event.type == sf::Event::TextEntered) {
            if (isdigit(event.text.unicode)) {
                placing = event.text.unicode - '0';
                cout << "Placing " << placing << endl;
            }
        }
    }
    gstate->gm->map().update(false);
    for (auto& rm : gstate->rms) {
        rm.update();
    }
    return {};
}

void EditorStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect({0, 0}, Point(window.getSize()) / SCALEUP / gstate->dpi_scaling_factor));
    window.setView(view);

    gstate->rms[0].render(window);
}

void StageManager::update(sf::RenderWindow& window) {
    if (!current_stage) return;
    auto new_stage = current_stage->update(window);
    if (new_stage) {
        current_stage = move(new_stage);
        update(window);
    }
}

void StageManager::render(sf::RenderWindow& window) {
    if (!current_stage) return;
    current_stage->render(window);
}

void StageManager::start_stage(unique_ptr<Stage>&& stage) {
    current_stage = forward<unique_ptr<Stage>>(stage);
}
