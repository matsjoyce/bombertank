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
#include "map.hpp"
#include "rendermap.hpp"
#include "objects/player.hpp"
#include <fstream>
#include <thread>
#include <filesystem>
#include <list>

namespace fs = std::filesystem;
using namespace std;

struct GameState {
    ServerMap sm;
    vector<pair<int, int>> starting_positions;
    vector<objptr> players;
    vector<RenderMap> rms;
    thread thr;
    sf::Font font;
    int dpi_scaling_factor = 1;
    GameState() : sm(), thr([this]{return sm.run();}) {
        font.loadFromFile("data/fonts/font.pcf");
        const_cast<sf::Texture&>(font.getTexture(12)).setSmooth(false);
    }
    ~GameState() {
        sm.halt();
        thr.join();
    }
};

unique_ptr<Stage> Stage::update(sf::RenderWindow& /*window*/) {
    return {};
}

void Stage::render(sf::RenderWindow& /*window*/) {
}

LoadStage::LoadStage(bool is_editor) : gstate(make_unique<GameState>()), load_editor(is_editor) {
}

unique_ptr<Stage> LoadStage::update(sf::RenderWindow& window) {
    if (window.getSize().x > 2000) {
        gstate->dpi_scaling_factor = 2;
    }
    if (load_editor) {
        auto f = ifstream("map_save.btm");
        load_objects_from_file(f, gstate->sm);
        gstate->sm.pause();
        auto es1 = make_unique<EventServer>(), es2 = make_unique<EventServer>();
        es1->connect(es2.get());
        es2->connect(es1.get());
        gstate->rms.emplace_back(move(es1), 0);
        gstate->sm.add_controller(0, move(es2));
        return make_unique<EditorStage>(move(gstate));
    }
    return make_unique<SelectPlayMapStage>(move(gstate));
}

std::unique_ptr<Stage> LoadStage::create(bool is_editor/*=false*/) {
    return make_unique<LoadStage>(is_editor);
}

SelectPlayMapStage::SelectPlayMapStage(std::unique_ptr<GameState> gs) : gstate(move(gs)) {
    load_maps("maps");
    sort(maps.begin(), maps.end());
}

unique_ptr<Stage> SelectPlayMapStage::update(sf::RenderWindow& window) {
    if (window.getSize().x > 2000) {
        gstate->dpi_scaling_factor = 2;
    }
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Enter) {
                auto f = ifstream(maps[current_index]);
                gstate->players = load_objects_from_file(f, gstate->sm);
                int i = 0;
                for (auto& player : gstate->players) {
                    gstate->starting_positions.emplace_back(make_pair(player->x(), player->y()));
                    player->set_side(i);

                    auto es1 = make_unique<EventServer>(), es2 = make_unique<EventServer>();
                    es1->connect(es2.get());
                    es2->connect(es1.get());
                    gstate->rms.emplace_back(move(es1), i);
                    gstate->sm.add_controller(i, move(es2));
                    ++i;
                };
                return make_unique<PlayStage>(move(gstate));
            }
            else if (event.key.code == sf::Keyboard::Down) {
                current_index = (current_index + 1) % maps.size();
            }
            else if (event.key.code == sf::Keyboard::Up) {
                current_index = (current_index - 1 + maps.size()) % maps.size();
            }
        }
    }
    return {};
}

void SelectPlayMapStage::load_maps(std::string dir) {
    for (auto& item : fs::directory_iterator(dir)) {
        if (item.is_directory()) {
            load_maps(item.path());
        }
        else if (item.is_regular_file() && item.path().extension() == ".btm") {
            maps.push_back(item.path());
        }
    }
}

void SelectPlayMapStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(window.getSize() / 4u)));
    view.setViewport(sf::FloatRect(0, 0, 1.0, 1.0));
    window.setView(view);

    int y = 0;
    for (auto& map : maps) {
        sf::Text txt(map, gstate->font, 12);
        txt.setPosition(sf::Vector2f(6, 6 + 14 * y));
        txt.setFillColor(y == current_index ? sf::Color::Red : sf::Color::Black);
        window.draw(txt);
        ++y;
    }
}

PlayStage::PlayStage(std::unique_ptr<GameState> gs) : gstate(move(gs)), text("Paused", gstate->font, 12) {
}

unique_ptr<Stage> PlayStage::update(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::P) {
                cout << "PAUSE/RESUME" << endl;
                if (!gstate->sm.is_paused()) {
                    gstate->sm.pause();
                }
                else {
                    gstate->sm.resume();
                }
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
    int num_alive = 0;
    for (auto& player : gstate->players) {
        num_alive += player->alive();
    }
    if (num_alive <= 1) {
        gstate->sm.pause();
        return make_unique<GameOverStage>(move(gstate));
    }
    return {};
}

void draw_darkbg_text(sf::View& view, sf::RenderTarget& window, unique_ptr<GameState>& gstate, int darkness, sf::Text& text) {
    const unsigned int scaleup = 12 * gstate->dpi_scaling_factor;

    view.reset(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(window.getSize() / scaleup)));
    view.setViewport(sf::FloatRect(0, 0, 1.0, 1.0));
    window.setView(view);

    sf::RectangleShape drect(sf::Vector2f(window.getSize() / scaleup));
    drect.setFillColor(sf::Color(0, 0, 0, min(200, darkness)));
    window.draw(drect);

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
                   textRect.top  + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(window.getSize() / 2u / scaleup));
    text.setFillColor(sf::Color(255, 0, 0, darkness));
    window.draw(text);
}

void PlayStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect(0, 0, window.getSize().x / 2 / gstate->dpi_scaling_factor, window.getSize().y / 4 / gstate->dpi_scaling_factor));

    view.setViewport(sf::FloatRect(0, 0, 1.0, 0.5));
    window.setView(view);
    gstate->rms[0].render(window);

    view.setViewport(sf::FloatRect(0, 0.5, 1.0, 0.5));
    window.setView(view);
    gstate->rms[1].render(window);

    if (gstate->sm.is_paused()) {
        if (darkness < 255) {
            ++darkness;
        }

        draw_darkbg_text(view, window, gstate, darkness, text);
    }
    else {
        darkness = 0;
    }
}

GameOverStage::GameOverStage(unique_ptr<GameState> gs) : gstate(move(gs)), text("Game Over!", gstate->font, 12) {
}

void GameOverStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect(0, 0, window.getSize().x / 2 / gstate->dpi_scaling_factor, window.getSize().y / 4 / gstate->dpi_scaling_factor));

    view.setViewport(sf::FloatRect(0, 0, 1.0, 0.5));
    window.setView(view);
    gstate->rms[0].render(window);

    view.setViewport(sf::FloatRect(0, 0.5, 1.0, 0.5));
    window.setView(view);
    gstate->rms[1].render(window);

    if (darkness < 255) {
        ++darkness;
    }

    draw_darkbg_text(view, window, gstate, darkness, text);
}

unique_ptr<Stage> GameOverStage::update(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            for (unsigned int i = 0; i < gstate->players.size(); ++i) {
                if (!gstate->players[i]->alive()) {
                    auto obj = gstate->players[i] = gstate->sm.add(Player::TYPE);
                    obj->place(gstate->starting_positions[i].first, gstate->starting_positions[i].second);
                    obj->set_side(i);
                }
            }
            gstate->sm.resume();
            return make_unique<PlayStage>(move(gstate));
        }
    }
    for (auto& rm : gstate->rms) {
        rm.update();
    }
    return {};
}

EditorStage::EditorStage(std::unique_ptr<GameState> gs) : gstate(move(gs)) {
}

unique_ptr<Stage> EditorStage::update(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            int x = event.mouseButton.x / STANDARD_OBJECT_SIZE / 2 / gstate->dpi_scaling_factor;
            int y = event.mouseButton.y / STANDARD_OBJECT_SIZE / 2 / gstate->dpi_scaling_factor;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
                int start_x = min(x, last_x), end_x = max(x, last_x);
                int start_y = min(y, last_y), end_y = max(y, last_y);
                for (int x_ = start_x; x_ <= end_x; ++x_) {
                    for (int y_ = start_y; y_ <= end_y; ++y_) {
                        if (x_ != last_x || y_ != last_y) {
                            auto obj = gstate->sm.add(placing);
                            obj->place_on_tile(x_, y_);
                        }
                    }
                }
            }
            else {
                auto obj = gstate->sm.add(placing);
                obj->place_on_tile(x, y);
            }
            last_x = x;
            last_y = y;
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.control && event.key.code == sf::Keyboard::S) {
                cout << "Saving" << endl;
                ofstream f("map_save.btm");
                gstate->sm.save_objects_to_map(f);
                f.close();
            }
            else if (event.key.code == sf::Keyboard::Delete) {
                auto pos = sf::Mouse::getPosition(window);
                int x = pos.x / STANDARD_OBJECT_SIZE / 2 / gstate->dpi_scaling_factor;
                int y = pos.y / STANDARD_OBJECT_SIZE / 2 / gstate->dpi_scaling_factor;
                if (event.key.shift) {
                    int start_x = min(x, last_dx), end_x = max(x, last_dx);
                    int start_y = min(y, last_dy), end_y = max(y, last_dy);

                    for (auto& obj : gstate->sm.collides(start_x * STANDARD_OBJECT_SIZE, start_y * STANDARD_OBJECT_SIZE,
                                                         (end_x - start_x + 1) * STANDARD_OBJECT_SIZE,
                                                         (end_y - start_y + 1) * STANDARD_OBJECT_SIZE)) {
                        obj->destroy();
                    }
                }
                else {
                    for (auto& obj : gstate->sm.collides(x * STANDARD_OBJECT_SIZE, y * STANDARD_OBJECT_SIZE, STANDARD_OBJECT_SIZE, STANDARD_OBJECT_SIZE)) {
                        obj->destroy();
                    }
                }
                last_dx = x;
                last_dy = y;
            }
        }
        if (event.type == sf::Event::TextEntered) {
            if (isdigit(event.text.unicode)) {
                placing = event.text.unicode - '0';
                cout << "Placing " << placing << endl;
            }
        }
    }
    for (auto& rm : gstate->rms) {
        rm.update();
    }
    return {};
}

void EditorStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect(0, 0, window.getSize().x / 2 / gstate->dpi_scaling_factor, window.getSize().y / 2 / gstate->dpi_scaling_factor));
    window.setView(view);

    sf::Texture tex = gstate->rms[0].load_texture("data/images/blank.png");
    tex.setRepeated(true);
    sf::Sprite spr(tex);
    spr.setTextureRect(sf::IntRect(0, 0, window.getSize().x / 2 / gstate->dpi_scaling_factor, window.getSize().y / 2 / gstate->dpi_scaling_factor));
    window.draw(spr);

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
