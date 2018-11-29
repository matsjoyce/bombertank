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

#include "playeritem.hpp"
#include <iostream>
#include "bomb.hpp"
#include "walls.hpp"
#include "attackutils.hpp"

using namespace std;

enum class PIRenderMessage : unsigned int {
    UPDATE, DESTROY, START, END
};

void PlayerItem::attach(std::shared_ptr<Player> pl) {
    player_ = pl;
}

void PlayerItem::drop() {
    player_ = {};
}

std::shared_ptr<Player> PlayerItem::player() {
    return player_.lock();
}

void PlayerItem::start() {
    active_ = true;
}

void PlayerItem::end() {
    active_ = false;
}

void PlayerItem::render_handle(msgpackvar&& m) {
    cout << "Unhandled event " << m["itype"].as_uint64_t() << " for item " << type() << endl;
}

void PlayerItem::merge_with(std::shared_ptr<PlayerItem> /*item*/) {
}

UsesPlayerItem::UsesPlayerItem(unsigned int uses_) : uses(uses_) {
}

void UsesPlayerItem::merge_with(std::shared_ptr<PlayerItem> item) {
    auto i = dynamic_cast<UsesPlayerItem*>(item.get());
    uses += i->uses;
    send_update();
}

void UsesPlayerItem::render_handle(msgpackvar&& m) {
    switch (static_cast<PIRenderMessage>(m["itype"].as_uint64_t())) {
        case PIRenderMessage::UPDATE: {
            uses = m["uses"].as_uint64_t();
            break;
        }
        default: PlayerItem::render_handle(std::move(m));
    }
}

void UsesPlayerItem::send_update() {
    msgpackvar m;
    m["itype"] = as_ui(PIRenderMessage::UPDATE);
    m["uses"] = uses;
    player()->item_msg(std::move(m), type());
}

BombItem::BombItem() : UsesPlayerItem(3) {
}

void BombItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player()->render_map()->load_texture("data/images/bomb.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(uses), player()->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void BombItem::start() {
    PlayerItem::start();
    cout << "DROP_BOMB " << uses << endl;
    if (uses) {
        --uses;
        send_update();
        auto obj = player()->server_map()->add(TimedBomb::TYPE);
        obj->place(player()->tcx(), player()->tcy());
        weak_ptr<BombItem> weak_this = dynamic_pointer_cast<BombItem>(shared_from_this());
        obj->destroyed.connect([weak_this] {
            if (auto obj = weak_this.lock()) {
                ++obj->uses;
                obj->send_update();
            }
        });
    }
}

CrateItem::CrateItem() : UsesPlayerItem(8) {
}

void CrateItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player()->render_map()->load_texture("data/images/pwall.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(uses), player()->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void CrateItem::start() {
    PlayerItem::start();
    cout << "DROP_WALL " << uses << endl;
    if (uses) {
        --uses;
        send_update();
        auto obj = player()->server_map()->add(PlacedWall::TYPE);
        obj->place(player()->tcx(), player()->tcy());
        weak_ptr<CrateItem> weak_this = dynamic_pointer_cast<CrateItem>(shared_from_this());
        obj->destroyed.connect([weak_this] {
            if (auto obj = weak_this.lock()) {
                ++obj->uses;
                obj->send_update();
            }
        });
    }
}

MineItem::MineItem() : UsesPlayerItem(2) {
}

void MineItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player()->render_map()->load_texture("data/images/mine.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(uses), player()->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void MineItem::start() {
    PlayerItem::start();
    cout << "DROP_MINE " << uses << endl;
    if (uses) {
        --uses;
        send_update();
        auto obj = player()->server_map()->add(Mine::TYPE);
        obj->place(player()->tcx(), player()->tcy());
        obj->set_side(player()->side());
    }
}

ChargeItem::ChargeItem() : UsesPlayerItem(5) {
}

void ChargeItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player()->render_map()->load_texture("data/images/charge.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(uses), player()->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void ChargeItem::start() {
    PlayerItem::start();
    cout << "DROP_CHARGE " << uses << endl;
    if (uses) {
        --uses;
        send_update();
        auto obj = player()->server_map()->add(StaticBomb::TYPE);
        obj->place(player()->tcx(), player()->tcy());
        obj->set_side(player()->side());
    }
}

LaserItem::LaserItem() : UsesPlayerItem(50) {
}

void LaserItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player()->render_map()->load_texture("data/images/laser_icon.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(uses), player()->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void LaserItem::start() {
    PlayerItem::start();
    cout << "LASER " << uses << endl;
    if (uses) {
        --uses;
        auto ori = player()->orientation();
        auto x = player()->x() + dx(ori) * (player()->width() / 2);
        auto y = player()->y() + dy(ori) * (player()->height() / 2);
        auto dist = progressive_kill_in_direction(player()->server_map(), x, y, 4, STANDARD_OBJECT_SIZE * 10, ori, 10, DamageType::HEAT);

        msgpackvar m;
        m["itype"] = as_ui(PIRenderMessage::START);
        m["dist"] = dist;
        m["ori"] = as_ui(ori);
        m["x"] = x;
        m["y"] = y;
        m["uses"] = uses;
        player()->item_msg(std::move(m), type());
    }
}

class LaserEffect : public Effect {
    unsigned int dist;
public:
    unsigned int layer() override {
        return 5;
    }
    LaserEffect(RenderMap* map_, unsigned int id_, int x_, int y_, Orientation::Orientation ori, unsigned int dist_)
        : Effect(map_, id_, x_, y_, ori), dist(dist_) {
        sound.setBuffer(map->load_sound_buf("data/sounds/laser-small.wav"));
        sound.play();
    }
    void render(sf::RenderTarget& rt) override {
        if (time_left) {
            auto tex = map->load_texture("data/images/laser.png");
            tex.setRepeated(true);
            sf::Sprite sp(tex);
            sp.setOrigin(sf::Vector2f(sp.getTextureRect().width / 2, dist));
            sp.setPosition(sf::Vector2f(x, y));
            sp.setTextureRect({0, 0, sp.getTextureRect().width, static_cast<int>(dist)});
            sp.setRotation(angle(orientation));
            rt.draw(sp);
        }
    }
    void update() override {
        if (time_left) {
            --time_left;
        }
        if (!time_left && sound.getStatus() == sf::Sound::Stopped) {
            destroy();
        }
    }
private:
    int time_left = 5;
    sf::Sound sound;
};

void LaserItem::render_handle(msgpackvar&& m) {
    switch (static_cast<PIRenderMessage>(m["itype"].as_uint64_t())) {
        case PIRenderMessage::START: {
            player()->render_map()->add_effect<LaserEffect>(extract_int(m["x"]), extract_int(m["y"]),
                                                            static_cast<Orientation::Orientation>(m["ori"].as_uint64_t()), m["dist"].as_uint64_t());
            uses = m["uses"].as_uint64_t();
            break;
        }
        default: UsesPlayerItem::render_handle(std::move(m));
    }
}

map<unsigned int, function<shared_ptr<PlayerItem>()>> load_player_items() {
    decltype(load_player_items()) ret;
    ret[BombItem::TYPE] = make_shared<BombItem>;
    ret[CrateItem::TYPE] = make_shared<CrateItem>;
    ret[MineItem::TYPE] = make_shared<MineItem>;
    ret[ChargeItem::TYPE] = make_shared<ChargeItem>;
    ret[LaserItem::TYPE] = make_shared<LaserItem>;
    return ret;
}
