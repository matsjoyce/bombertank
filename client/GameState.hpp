#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <QObject>
#include <QQmlEngine>

#include "common/TcpMessageSocket.hpp"
#include "objects/Base.hpp"
#include "objects/TankControl.hpp"


class BaseGameState : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

   public:
    using QObject::QObject;

    virtual const std::map<int, std::unique_ptr<BaseObjectState>>& snapshot() const = 0;
};

class GameServer;

class GameState : public BaseGameState {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

    GameServer* _server;

    std::map<int, std::unique_ptr<BaseObjectState>> _objectStates;

   public:
    GameState(GameServer* parent);

    const std::map<int, std::unique_ptr<BaseObjectState>>& snapshot() const override;

   public slots:
    void handleMessage(int id, Message msg);
    void setControlState(int objectId, TankControlState* controlState);
    void exitGame();

   signals:
    void sendMessage(Message msg);
    void attachToObject(int id);
};

class EditorGameState : public BaseGameState {
    Q_OBJECT
    QML_ELEMENT

    std::map<int, std::unique_ptr<BaseObjectState>> _objectStates;
    int _nextId = 1;

   public:
    using BaseGameState::BaseGameState;

    const std::map<int, std::unique_ptr<BaseObjectState>>& snapshot() const override;
    Q_INVOKABLE void clear();
    Q_INVOKABLE int addObject(int type, float x, float y);
    Q_INVOKABLE void save(QUrl fname) const;
    Q_INVOKABLE static EditorGameState* load(QUrl fname);
};

#endif  // GAME_STATE_HPP
