#include "GameMode.hpp"

#include<QDebug>

void GameMode::setGame(Game* game) { _game = game; }

void IndividualDeathMatch::setGame(Game* game) {
    GameMode::setGame(game);

    if (this->game()) {
        connect(this->game(), &Game::playerConnected, this, &IndividualDeathMatch::_onPlayerConnected);
        connect(this->game(), &Game::playerAttachedObjectDied, this,
                &IndividualDeathMatch::_onPlayerAttachedObjectDied);
    }
}

void IndividualDeathMatch::_onPlayerConnected(int id, Message msg) {
    auto startZones = game()->objectsOfType(constants::ObjectType::START_ZONE);

    if (!startZones.size()) {
        qWarning() << "No start zones";
        return;
    }
    if (!_playerToSide.count(id)) {
        _playerToSide[id] = _nextSideAssignment++ % startZones.size();
    }
    auto side = _playerToSide[id];
    auto startZone = game()->object(startZones[side]);
    auto tank = game()->addObject(constants::ObjectType::TANK, startZone->body()->GetPosition(), startZone->body()->GetAngle(), {0, 0});
    if (tank) {
        auto [tankId, tankObj] = *tank;
        tankObj->setSide(side);
        tankObj->handleMessage(msg);
        game()->attachPlayerToObject(id, tankId);
    }
}

void IndividualDeathMatch::_onPlayerAttachedObjectDied(int id) {}
