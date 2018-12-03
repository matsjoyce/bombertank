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

#ifndef STAGEMANAGER_HPP
#define STAGEMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "point.hpp"
#include "signal.hpp"

class Stage {
public:
    virtual std::unique_ptr<Stage> update(sf::RenderWindow& window);
    virtual void render(sf::RenderWindow& window);
    virtual ~Stage() = default;
};

struct GameState;

class LoadStage : public Stage {
    std::unique_ptr<GameState> gstate;
    bool load_editor;
public:
    LoadStage(bool is_editor=false);
    std::unique_ptr<Stage> update(sf::RenderWindow& window) override;
    static std::unique_ptr<Stage> create(bool is_editor=false);
    std::vector<std::string> load_maps(std::string dir);
};

class SelectStage : public Stage {
    std::unique_ptr<GameState> gstate;
    std::vector<std::string> values;
    int current_index = 0;
public:
    std::function<std::unique_ptr<Stage>(unsigned int, std::string, std::unique_ptr<GameState>&&)> on_selected;
    SelectStage(std::unique_ptr<GameState> gs, std::vector<std::string> vals);
    std::unique_ptr<Stage> update(sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
};

class PlayStage : public Stage {
    std::unique_ptr<GameState> gstate;
    sf::Text text;
    int darkness = 0;
public:
    PlayStage(std::unique_ptr<GameState> gs);
    std::unique_ptr<Stage> update(sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
};

class GameOverStage : public Stage {
    std::unique_ptr<GameState> gstate;
    sf::Text text;
    int darkness = 0;
public:
    GameOverStage(std::unique_ptr<GameState> gs);
    std::unique_ptr<Stage> update(sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
};

class EditorStage : public Stage {
    std::unique_ptr<GameState> gstate;
    int placing = 1;
    Point last_pos;
    std::string fname_;
public:
    EditorStage(std::unique_ptr<GameState> gs, std::string fname);
    std::unique_ptr<Stage> update(sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
};

class StageManager {
    std::unique_ptr<Stage> current_stage;
public:
    ~StageManager() = default;
    void update(sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
    void start_stage(std::unique_ptr<Stage>&& stage);
};

#endif // STAGEMANAGER_HPP
