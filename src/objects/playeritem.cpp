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
    UPDATE, DESTROY, START, END, FIRE
};

void PlayerItem::attach(std::shared_ptr<Player> pl) {
    player_ = pl;
    on_attach.emit();
}

void PlayerItem::drop() {
    player_ = {};
    on_detatch.emit();
}

std::shared_ptr<Player> PlayerItem::player() {
    return player_.lock();
}

bool PlayerItem::can_activate() {
    return true;
}

void PlayerItem::update() {
}

void PlayerItem::try_start() {
    if (active_) return;
    if (can_activate()) {
        active_ = true;
        msgpackvar m;
        m["itype"] = as_ui(PIRenderMessage::START);
        player()->item_msg(std::move(m), type());
        start();
    }
}

void PlayerItem::try_end() {
    if (!active_) return;
    active_ = false;
    msgpackvar m;
    m["itype"] = as_ui(PIRenderMessage::END);
    player()->item_msg(std::move(m), type());
    end();
}

void PlayerItem::start() {
}

void PlayerItem::end() {
}

void PlayerItem::render_handle(msgpackvar&& m) {
    switch (static_cast<PIRenderMessage>(m["itype"].as_uint64_t())) {
        case PIRenderMessage::START: {
            active_ = true;
            break;
        }
        case PIRenderMessage::END: {
            active_ = false;
            break;
        }
        default: {
            cout << "Unhandled event " << m["itype"].as_uint64_t() << " for item " << type() << endl;
        }
    }
}

void PlayerItem::merge_with(std::shared_ptr<PlayerItem> /*item*/) {
}

unsigned int PlayerItem::damage_intercept(unsigned int damage, DamageType /*dt*/) {
    return damage;
}

void PlayerItem::render_overlay(sf::RenderTarget& /*rt*/) {
}

UsesPlayerItem::UsesPlayerItem() : uses(-1) {
    on_attach.connect([this]{
        uses = min(uses, max_uses());
    });
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

bool UsesPlayerItem::can_activate() {
    return uses;
}

void UsedPlayerItem::render_handle(msgpackvar&& m) {
    switch (static_cast<PIRenderMessage>(m["itype"].as_uint64_t())) {
        case PIRenderMessage::UPDATE: {
            used = m["used"].as_uint64_t();
            break;
        }
        default: PlayerItem::render_handle(std::move(m));
    }
}

void UsedPlayerItem::send_update() {
    msgpackvar m;
    m["itype"] = as_ui(PIRenderMessage::UPDATE);
    m["used"] = used;
    player()->item_msg(std::move(m), type());
}

bool UsedPlayerItem::can_activate() {
    return used < max_uses();
}

unsigned int BombItem::max_uses() {
    return player()->level() >= 7 ? 2 : 1;
}


void BombItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player()->render_map()->load_texture("data/images/bomb.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(max_uses() - used), player()->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void BombItem::start() {
    ++used;
    send_update();
    auto pl = player();
    auto obj = pl->server_map()->add(TimedBomb::TYPE);
    obj->set_nw_corner(pl->center().to_tile().from_tile());
    obj->_generate_move();
    dynamic_pointer_cast<StaticBomb>(obj)->set_power(pl->level() >= 2 ? 2 : 1);
    weak_ptr<BombItem> weak_this = dynamic_pointer_cast<BombItem>(shared_from_this());
    obj->destroyed.connect([weak_this] {
        if (auto obj = weak_this.lock()) {
            --obj->used;
            obj->send_update();
        }
    });
}

unsigned int CrateItem::max_uses() {
    return 8;
}

void CrateItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player()->render_map()->load_texture("data/images/pwall.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(max_uses() - used), player()->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void CrateItem::start() {
    ++used;
    send_update();
    auto pl = player();
    auto obj = pl->server_map()->add(PlacedWall::TYPE);
    obj->set_nw_corner(pl->center().to_tile().from_tile());
    obj->_generate_move();
    weak_ptr<CrateItem> weak_this = dynamic_pointer_cast<CrateItem>(shared_from_this());
    obj->destroyed.connect([weak_this] {
        if (auto obj = weak_this.lock()) {
            --obj->used;
            obj->send_update();
        }
    });
}

unsigned int MineItem::max_uses() {
    return 4;
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
    --uses;
    send_update();
    auto pl = player();
    auto obj = pl->server_map()->add(Mine::TYPE);
    obj->set_nw_corner(pl->center().to_tile().from_tile());
    obj->_generate_move();
    obj->set_side(pl->side());
    dynamic_pointer_cast<StaticBomb>(obj)->set_power(pl->level() >= 6 ? 2 : 1);
}

unsigned int ChargeItem::max_uses() {
    return 5;
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
    --uses;
    send_update();
    auto pl = player();
    auto obj = pl->server_map()->add(StaticBomb::TYPE);
    obj->set_nw_corner(pl->center().to_tile().from_tile());
    obj->_generate_move();
    dynamic_pointer_cast<StaticBomb>(obj)->set_power(pl->level() >= 4 ? 2 : 1);
}

unsigned int LaserItem::max_uses() {
    return 10;
}

void LaserItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player()->render_map()->load_texture("data/images/laser_icon.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(uses), player()->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void LaserItem::update() {
    if (warmup) {
        --warmup;
    }
    else if (active()) {
        warmup = 4;

        --uses;
        auto ori = player()->orientation();
        auto pos = player()->dir_center(ori);
        auto dist = progressive_kill_in_direction(player()->server_map(), pos, 4, STANDARD_OBJECT_SIZE * 10, ori, 10, DamageType::HEAT);

        msgpackvar m;
        m["itype"] = as_ui(PIRenderMessage::FIRE);
        m["dist"] = dist;
        m["ori"] = as_ui(ori);
        m["x"] = pos.x;
        m["y"] = pos.y;
        m["uses"] = uses;
        player()->item_msg(std::move(m), type());
        if (!uses) {
            try_end();
        }
    }
}

class LaserEffect : public Effect {
    unsigned int dist;
public:
    unsigned int layer() override {
        return 5;
    }
    LaserEffect(RenderMap* map_, unsigned int id_, Point pos_, Orientation::Orientation ori, unsigned int dist_)
        : Effect(map_, id_, pos_, ori), dist(dist_) {
        sound.setBuffer(map->load_sound_buf("data/sounds/laser-small.wav"));
        sound.play();
    }
    void render(sf::RenderTarget& rt) override {
        if (time_left) {
            auto tex = map->load_texture("data/images/laser.png");
            tex.setRepeated(true);
            sf::Sprite sp(tex);
            sp.setOrigin(sf::Vector2f(sp.getTextureRect().width / 2, dist));
            sp.setPosition(pos);
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
        case PIRenderMessage::FIRE: {
            player()->render_map()->add_effect<LaserEffect>(Point(extract_int(m["x"]), extract_int(m["y"])),
                                                            static_cast<Orientation::Orientation>(m["ori"].as_uint64_t()), m["dist"].as_uint64_t());
            uses = m["uses"].as_uint64_t();
            break;
        }
        default: UsesPlayerItem::render_handle(std::move(m));
    }
}

unsigned int ShieldItem::max_uses() {
    return 100;
}

void ShieldItem::render(sf::RenderTarget& rt, sf::Vector2f position) {
    sf::Sprite spr(player()->render_map()->load_texture("data/images/shield_icon.png"));
    spr.setPosition(position);
    rt.draw(spr);
    sf::Text txt(to_string(uses), player()->render_map()->load_font("data/fonts/font.pcf"), 12);
    txt.setPosition(position);
    rt.draw(txt);
}

void ShieldItem::render_handle(msgpackvar&& m) {
    switch (static_cast<PIRenderMessage>(m["itype"].as_uint64_t())) {
        case PIRenderMessage::FIRE: {
            uses = m["uses"].as_uint64_t();
            glow = 500;
            break;
        }
        default: UsesPlayerItem::render_handle(std::move(m));
    }
}

unsigned int ShieldItem::damage_intercept(unsigned int damage, DamageType /*dt*/) {
    auto amount = min(uses, damage);
    uses -= amount;

    msgpackvar m;
    m["itype"] = as_ui(PIRenderMessage::FIRE);
    m["uses"] = uses;
    player()->item_msg(std::move(m), type());

    if (!uses) {
        try_end();
    }
    return damage - amount;
}

void ShieldItem::render_overlay(sf::RenderTarget& rt) {
    if (glow) {
        sf::Sprite spr(player()->render_map()->load_texture("data/images/shield.png"));
        player()->position_sprite(spr);
        spr.setColor(sf::Color(255, 255, 255, min(255, glow)));
        rt.draw(spr);
        glow -= 10;
    }
}

map<unsigned int, function<shared_ptr<PlayerItem>()>> load_player_items() {
    decltype(load_player_items()) ret;
    ret[BombItem::TYPE] = make_shared<BombItem>;
    ret[CrateItem::TYPE] = make_shared<CrateItem>;
    ret[MineItem::TYPE] = make_shared<MineItem>;
    ret[ChargeItem::TYPE] = make_shared<ChargeItem>;
    ret[LaserItem::TYPE] = make_shared<LaserItem>;
    ret[ShieldItem::TYPE] = make_shared<ShieldItem>;
    return ret;
}
