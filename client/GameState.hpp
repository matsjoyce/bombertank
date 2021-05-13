#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <QObject>

#include "../common/TcpMessageSocket.hpp"
#include "objects/Base.hpp"
#include "objects/TankControl.hpp"

class GameServer;

class GameState : public QObject {
    Q_OBJECT

    GameServer* _server;

    std::map<int, std::unique_ptr<BaseObjectState>> _objectStates;

   public:
    GameState(GameServer* parent);

    const std::map<int, std::unique_ptr<BaseObjectState>>& snapshot() const;

   public slots:
    void handleMessage(int id, Message msg);
    void setControlState(int objectId, TankControlState* controlState);

   signals:
    void sendMessage(Message msg);
};

#endif  // GAME_STATE_HPP