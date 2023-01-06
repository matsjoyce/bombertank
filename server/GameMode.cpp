#include "GameMode.hpp"
#include "objects/Tank.hpp"

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

void IndividualDeathMatch::_onPlayerConnected(int id, std::shared_ptr<bt_messages::ToServerMessage> msg) {
    auto startZones = game()->objectsOfType(static_cast<int>(constants::ObjectType::START_ZONE));

    if (!startZones.size()) {
        qWarning() << "No start zones";
        return;
    }
    if (!_playerToSide.count(id)) {
        _playerToSide[id] = _nextSideAssignment++ % startZones.size();
        _maxDeaths += 2;
    }
    if (auto existingObj = game()->attachedObjectForPlayer(id)) {
        game()->attachPlayerToObject(id, *existingObj);
    }
    else {
        auto side = _playerToSide[id];
        auto startZone = game()->object(startZones[side]);
        auto tank = game()->addObject(constants::ObjectType::TANK, startZone->body()->GetPosition(), startZone->body()->GetAngle(), {0, 0});
        if (tank) {
            if (auto tankObj = dynamic_cast<TankState*>(tank->second)) {
                tankObj->setSide(side + 1);
                auto& tank_modules = msg->join_game().tank_modules();
                tankObj->setModules(std::vector<int>(tank_modules.begin(), tank_modules.end()));
            }
            game()->attachPlayerToObject(id, tank->first);
        }
    }
    _sendDeathStats();
}

void IndividualDeathMatch::_onPlayerAttachedObjectDied(int id) {
    if (_playerToSide.count(id)) {
        ++_sideToDeaths[_playerToSide[id]];
        _sendDeathStats();
        if (_sideToDeaths[_playerToSide[id]] < _maxDeaths) {
            auto msg = std::make_shared<bt_messages::ToClientMessage>();
            msg->mutable_dead_can_rejoin();
            emit sendMessage(id, msg);
        }
        else {
            _checkGameOver();
        }
    }
}

void IndividualDeathMatch::_sendDeathStats() {
    for (auto& deaths : _sideToDeaths) {
        qDebug() << "For side" << deaths.first << deaths.second << "deaths";
    }
    for (auto& player : _playerToSide) {
        auto msg = std::make_shared<bt_messages::ToClientMessage>();
        auto game_mode_update = msg->mutable_game_mode_update();
        game_mode_update->set_lives_left(std::max(0, _maxDeaths - _sideToDeaths[player.second]));
        game_mode_update->set_lives_total(_maxDeaths);
        emit sendMessage(player.first, msg);
    }
}

void IndividualDeathMatch::_checkGameOver() {
    std::set<int> winners;
    for (auto& [side, deaths] : _sideToDeaths) {
        if (deaths < _maxDeaths) {
            winners.insert(side);
        }
    }
    for (auto& player : _playerToSide) {
        auto msg = std::make_shared<bt_messages::ToClientMessage>();
        auto game_over = msg->mutable_game_over();
        game_over->set_winner(winners.count(player.second) == 1);
        emit sendMessage(player.first, msg);
    }
    emit gameOver();
}
