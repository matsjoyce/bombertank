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
        _maxDeaths += 2;
    }
    auto side = _playerToSide[id];
    auto startZone = game()->object(startZones[side]);
    auto tank = game()->addObject(constants::ObjectType::TANK, startZone->body()->GetPosition(), startZone->body()->GetAngle(), {0, 0});
    if (tank) {
        auto [tankId, tankObj] = *tank;
        tankObj->setSide(side + 1);
        tankObj->handleMessage(msg);
        game()->attachPlayerToObject(id, tankId);
    }
    _sendDeathStats();
}

void IndividualDeathMatch::_onPlayerAttachedObjectDied(int id) {
    if (_playerToSide.count(id)) {
        ++_sideToDeaths[_playerToSide[id]];
        _sendDeathStats();
    }
}

void IndividualDeathMatch::_sendDeathStats() {
    for (auto& deaths : _sideToDeaths) {
        qDebug() << "For side" << deaths.first << deaths.second << "deaths";
    }
    for (auto& player : _playerToSide) {
        emit sendMessage(player.first, {{"cmd", "livesLeft"}, {"left", std::max(0, _maxDeaths - _sideToDeaths[player.second])}, {"total", _maxDeaths}});
    }
}

