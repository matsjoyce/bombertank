#include "mapgenerator.hpp"
#include "thirdparty/FastNoiseLite.hpp"
#include "objects/floors.hpp"
#include "objects/walls.hpp"
#include <iostream>

using namespace std;

ExperimentalStage::ExperimentalStage() {
    FastNoiseLite noise;
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);

    auto dim = 500u;
    img.create(dim, dim);
    for (auto x = 0u; x < dim; ++x) {
        for (auto y = 0u; y < dim; ++y) {
            int c = (noise.GetNoise(static_cast<float>(x), static_cast<float>(y)) / 2 + 0.5) * 255;
            img.setPixel(x, y, sf::Color(c, c, c));
        }
    }
}

unique_ptr<Stage> ExperimentalStage::update(sf::RenderWindow& window) {
    auto gstate = make_unique<GameState>();
    gstate->gm = make_unique<PVPGameManager>();
    auto gm = dynamic_cast<PVPGameManager*>(gstate->gm.get());
    auto& sm = dynamic_cast<ServerMap&>(gm->map());

    FastNoiseLite noise;
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);

    auto dim = 50;
    img.create(dim, dim);
    for (auto x = -dim; x < dim; ++x) {
        for (auto y = -dim; y < dim; ++y) {
            if (x == -dim || y == -dim || x == dim - 1 || y == dim - 1) {
                auto obj = sm.add(IndestructableWall::TYPE);
                obj->set_nw_corner(Point(x, y).from_tile());
                obj->_generate_move();
            }
            else {
                float n = noise.GetNoise(static_cast<float>(x), static_cast<float>(y)) / 2 + 0.5;
                if (n < 0.2) {
                    cout << x << " " << y << endl;
                    auto obj = sm.add(Lava::TYPE);
                    obj->set_nw_corner(Point(x, y).from_tile());
                    obj->_generate_move();
                }
                else if (n > 0.8) {
                    auto obj = sm.add(PlacedWall::TYPE);
                    obj->set_nw_corner(Point(x, y).from_tile());
                    obj->_generate_move();
                }
            }
        }
    }

    for (auto i = 0; i < 1; ++i) {
        gm->add_player(Point(0, 0).from_tile());
        auto es1 = make_unique<EventServer>(), es2 = make_unique<EventServer>();
        es1->connect(es2.get());
        es2->connect(es1.get());
        gstate->rms.emplace_back(move(es1), i);
        gm->add_controller(i, move(es2));
    };
    gm->start();
    return make_unique<PlayStage>(move(gstate));
}

void ExperimentalStage::render(sf::RenderWindow& window) {
    sf::View view;
    view.reset(sf::FloatRect({0, 0}, Point(window.getSize()) / SCALEUP / dpi_scaling_factor(window)));
    window.setView(view);

    sf::Sprite spr;
    sf::Texture texture;
    texture.loadFromImage(img);
    spr.setTexture(texture);
    window.draw(spr);
}
