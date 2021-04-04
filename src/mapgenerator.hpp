#ifndef MAPGENERATOR_HPP
#define MAPGENERATOR_HPP

#include "stagemanager.hpp"

class ExperimentalStage : public Stage {
    sf::Image img;
public:
    ExperimentalStage();
    std::unique_ptr<Stage> update(sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
};

#endif // MAPGENERATOR_HPP
