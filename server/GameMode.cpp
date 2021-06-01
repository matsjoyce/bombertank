#include "GameMode.hpp"

void GameMode::setGame(Game* game) { _game = game; }

void IndividualDeathMatch::setGame(Game* game) {
    GameMode::setGame(game);

    if (this->game()) {
        connect(this->game(), &Game::playerConnected, this, &IndividualDeathMatch::_onPlayerConnected);
        connect(this->game(), &Game::playerAttachedObjectDied, this,
                &IndividualDeathMatch::_onPlayerAttachedObjectDied);
    }
}

void IndividualDeathMatch::_onPlayerConnected(int id) {
    auto team = _playerToTeam[id] = _nextTeamAssignment++;
    auto objs = game()->objectsOnTeam(team);
    if (!objs.size()) {
        return;
    }
    game()->attachPlayerToObject(id, objs.front());
}

void IndividualDeathMatch::_onPlayerAttachedObjectDied(int id) {}