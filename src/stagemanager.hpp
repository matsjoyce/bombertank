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

class Stage {
public:
    virtual std::unique_ptr<Stage> update(sf::RenderWindow& window);
    virtual void render(sf::RenderWindow& window);
};

struct GameState;

class LoadStage : public Stage {
public:
    std::unique_ptr<Stage> update(sf::RenderWindow& window) override;
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

class StageManager {
    std::unique_ptr<Stage> current_stage;
public:
    StageManager();
    void update(sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
};

#endif // STAGEMANAGER_HPP
