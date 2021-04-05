#include "dropitem.hpp"
#include "player.hpp"
#include "effects.hpp"

using namespace std;

unsigned int DropItem::render_layer() {
    return 2;
}

void DropItem::update() {
    for (auto& obj : server_map()->collides(*this)) {
        if (auto pl = dynamic_pointer_cast<Player>(obj.second)) {
            apply(pl);
            destroy();
        }
    }
}

unsigned int DropItem::layer() {
    return 4;
}

unsigned int DropItem::take_damage(unsigned int /*damage*/, DamageType /*dt*/) {
    // You have no power here!
    return 0;
}

void HealthDropItem::render(sf::RenderTarget& rt) {
    sf::Sprite sp(render_map()->load_texture("data/images/health_box.png"));
    position_sprite(sp);
    rt.draw(sp);
}

void HealthDropItem::apply(shared_ptr<Player> player) {
    player->heal(100);
}
