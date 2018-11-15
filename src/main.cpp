#include <SFML/Graphics.hpp>
#include "stagemanager.hpp"


using namespace std;


int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "BomberTank");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    StageManager sm;
    while (window.isOpen()) {
        sm.update(window);
        sm.render(window);
    }
}
