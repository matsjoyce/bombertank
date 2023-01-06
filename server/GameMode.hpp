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

   signals:
    void sendMessage(int id, std::shared_ptr<bt_messages::ToClientMessage> msg);
    void gameOver();
};

class IndividualDeathMatch : public GameMode {
    Q_OBJECT

    std::map<int, int> _playerToSide;
    std::map<int, int> _sideToDeaths;
    int _nextSideAssignment = 0;
    int _maxDeaths = 0;
    void _sendDeathStats();
    void _checkGameOver();

   private slots:
    void _onPlayerConnected(int id, std::shared_ptr<bt_messages::ToServerMessage> msg);
    void _onPlayerAttachedObjectDied(int id);

   public:
    void setGame(Game* game) override;
};

#endif  // GAME_MODE_HPP
