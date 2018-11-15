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

using namespace std;

struct GameState {
    EventQueue eq1, eq2;
    ServerMap sm;
    RenderMap rm;
    vector<pair<int, int>> starting_positions;
    vector<pair<int, int>> current_positions;
    vector<objptr> players;
    thread thr;
    sf::RenderTexture black_tex;
    sf::Font font;
    GameState() : sm(eq1, eq2), rm(eq2, eq1), thr([this]{return sm.run();}) {
        black_tex.create(1, 1);
        black_tex.clear(sf::Color::Black);
        font.loadFromFile("font.pcf");
        const_cast<sf::Texture&>(font.getTexture(12)).setSmooth(false);
    }
    ~GameState() {
        sm.halt();
        thr.join();
    }
};

unique_ptr<Stage> Stage::update(sf::RenderWindow& window) {
    return {};
}

void Stage::render(sf::RenderWindow& rt) {
}

unique_ptr<Stage> LoadStage::update(sf::RenderWindow& window) {
    auto gs = make_unique<GameState>();
    auto f = ifstream("map.btm");
    gs->players = load_objects_from_file(f, gs->sm);
    int i = 0;
    for (auto& player : gs->players) {
        gs->starting_positions.emplace_back(make_pair(player->x(), player->y()));
        gs->current_positions.emplace_back(make_pair(player->x(), player->y()));
        dynamic_cast<Player*>(player.get())->set_num(i++);
    };
    return make_unique<PlayStage>(move(gs));
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
                if (!gstate->rm.is_paused()) {
                    gstate->rm.pause();
                }
                else {
                    gstate->rm.resume();
                }
            }
            else {
                gstate->rm.handle_keypress(event.key.code, true);
            }
        }
        if (event.type == sf::Event::KeyReleased) {
            gstate->rm.handle_keypress(event.key.code, false);
        }
    }
    gstate->rm.update();
    bool both_alive = true;
    if (gstate->players[0]->alive()) {
        auto player = gstate->rm.get_object(gstate->players[0]->id);
        if (player) {
            gstate->current_positions[0] = make_pair(player->x(), player->y());
        }
    }
    else both_alive = false;
    if (gstate->players[1]->alive()) {
        auto player = gstate->rm.get_object(gstate->players[1]->id);
        if (player) {
            gstate->current_positions[1] = make_pair(player->x(), player->y());
        }
    }
    else both_alive = false;
    if (!both_alive) {
        Message m;
        m.set_type(Message::PAUSE);
        gstate->rm.event(move(m));
        return make_unique<GameOverStage>(move(gstate));
    }
    return {};
}

void draw_darkbg_text(sf::View& view, sf::RenderTarget& window, unique_ptr<GameState>& gstate, int darkness, sf::Text& text) {
    constexpr const unsigned int scaleup = 24;

    view.reset(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(window.getSize() / scaleup)));
    view.setViewport(sf::FloatRect(0, 0, 1.0, 1.0));
    window.setView(view);

    sf::Sprite spr(gstate->black_tex.getTexture());
    spr.setColor(sf::Color(255, 255, 255, min(200, darkness)));
    spr.setPosition(sf::Vector2f(0, 0));
    spr.setScale(sf::Vector2f(window.getSize() / scaleup));
    window.draw(spr);

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
                   textRect.top  + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(window.getSize() / 2u / scaleup));
    text.setFillColor(sf::Color(255, 0, 0, darkness));
    window.draw(text);
}

void PlayStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect(0, 0, window.getSize().x / 4, window.getSize().y / 8));
    window.clear(sf::Color::White);

    view.setCenter(sf::Vector2f(gstate->current_positions[0].first, gstate->current_positions[0].second));
    view.setViewport(sf::FloatRect(0, 0, 1.0, 0.5));
    window.setView(view);
    gstate->rm.render(window);

    view.setCenter(sf::Vector2f(gstate->current_positions[1].first, gstate->current_positions[1].second));
    view.setViewport(sf::FloatRect(0, 0.5, 1.0, 0.5));
    window.setView(view);
    gstate->rm.render(window);

    if (gstate->rm.is_paused()) {
        if (darkness < 255) {
            ++darkness;
        }

        draw_darkbg_text(view, window, gstate, darkness, text);
    }
    else {
        darkness = 0;
    }

    window.display();
}

GameOverStage::GameOverStage(unique_ptr<GameState> gs) : gstate(move(gs)), text("Game Over!", gstate->font, 12) {
}

void GameOverStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect(0, 0, window.getSize().x / 4, window.getSize().y / 8));
    window.clear(sf::Color::White);

    view.setCenter(sf::Vector2f(gstate->current_positions[0].first, gstate->current_positions[0].second));
    view.setViewport(sf::FloatRect(0, 0, 1.0, 0.5));
    window.setView(view);
    gstate->rm.render(window);

    view.setCenter(sf::Vector2f(gstate->current_positions[1].first, gstate->current_positions[1].second));
    view.setViewport(sf::FloatRect(0, 0.5, 1.0, 0.5));
    window.setView(view);
    gstate->rm.render(window);

    if (darkness < 255) {
        ++darkness;
    }

    draw_darkbg_text(view, window, gstate, darkness, text);

    window.display();
}

unique_ptr<Stage> GameOverStage::update(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
            for (unsigned int i = 0; i < gstate->players.size(); ++i) {
                if (!gstate->players[i]->alive()) {
                    auto obj = gstate->players[i] = gstate->sm.add(Player::type);
                    obj->place(gstate->starting_positions[i].first, gstate->starting_positions[i].second);
                    gstate->current_positions.emplace_back(make_pair(obj->x(), obj->y()));
                    dynamic_cast<Player*>(obj.get())->set_num(i);
                }
            }
            Message m;
            m.set_type(Message::RESUME);
            gstate->rm.event(move(m));
            return make_unique<PlayStage>(move(gstate));
        }
    }
    gstate->rm.update();
    return {};
}

StageManager::StageManager() : current_stage(make_unique<LoadStage>()) {
}

void StageManager::update(sf::RenderWindow& window) {
    auto new_stage = current_stage->update(window);
    if (new_stage) {
        current_stage = move(new_stage);
        update(window);
    }
}

void StageManager::render(sf::RenderWindow& window) {
    current_stage->render(window);
}
