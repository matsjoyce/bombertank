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
#include "rendermap.hpp"
#include "gamemanager.hpp"
#include "objects/player.hpp"

constexpr unsigned int SCALEUP = 2;

class Stage {
public:
    virtual std::unique_ptr<Stage> update(sf::RenderWindow& window);
    virtual void render(sf::RenderWindow& window);
    virtual ~Stage() = default;
};

int dpi_scaling_factor(sf::RenderWindow& window);

struct GameState {
    std::unique_ptr<GameManager> gm;
    std::vector<Point> starting_positions;
    std::vector<std::shared_ptr<Player>> players;
    std::vector<RenderMap> rms;
    sf::Font font;
    GameState();
};

class LoadStage : public Stage {
    std::unique_ptr<GameState> gstate;
    bool load_editor;
    int current_index = 0;
    std::unique_ptr<Stage> selected(sf::RenderWindow& window);
    std::vector<std::string> load_maps(std::string dir);
public:
    LoadStage(bool is_editor=false);
    std::unique_ptr<Stage> update(sf::RenderWindow& window) override;
    static std::unique_ptr<Stage> create(bool is_editor=false);
    void render(sf::RenderWindow& window) override;
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
public:
    PlayStage(std::unique_ptr<GameState> gs);
    std::unique_ptr<Stage> update(sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
};

class EditorStage : public Stage {
    std::unique_ptr<GameState> gstate;
    unsigned int placing = 1;
    Point last_pos;
    std::string fname_;
    std::vector<unsigned int> placable_tiles;
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
