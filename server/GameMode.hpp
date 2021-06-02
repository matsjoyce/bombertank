#ifndef GAME_MODE_HPP
#define GAME_MODE_HPP

#include <QObject>

#include "Game.hpp"

class GameMode : public QObject {
    Q_OBJECT

    Game* _game;

   public:
    using QObject::QObject;

    virtual void setGame(Game* game);
    Game* game() { return _game; }
};

class IndividualDeathMatch : public GameMode {
    Q_OBJECT

    std::map<int, int> _playerToSide;
    int _nextSideAssignment = 1;
   private slots:
    void _onPlayerConnected(int id);
    void _onPlayerAttachedObjectDied(int id);

   public:
    void setGame(Game* game) override;
};

#endif  // GAME_MODE_HPP