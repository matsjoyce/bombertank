#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <QObject>
#include <QQmlEngine>
#include "box2d/box2d.h"

#include "common/TcpMessageSocket.hpp"
#include "objects/Base.hpp"
#include "objects/TankControl.hpp"


class BaseGameState : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

   public:
    using QObject::QObject;

    virtual const std::map<int, std::shared_ptr<BaseObjectState>>& snapshot() const = 0;
    virtual void cleanup() {}
};

class GameServer;

class GameState : public BaseGameState {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(int livesLeft READ livesLeft BINDABLE bindableLivesLeft)
    Q_PROPERTY(int livesTotal READ livesTotal BINDABLE bindableLivesTotal)

    Q_OBJECT_BINDABLE_PROPERTY(GameState, int, _livesLeftProp)
    Q_OBJECT_BINDABLE_PROPERTY(GameState, int, _livesTotalProp)

    GameServer* _server;
    int _id;

    std::map<int, std::shared_ptr<BaseObjectState>> _objectStates;

    std::shared_ptr<BaseObjectState>& _getOrCreateObject(const Message& msg);

   public:
    GameState(GameServer* parent, int id);

    int id() const { return _id; }
    const std::map<int, std::shared_ptr<BaseObjectState>>& snapshot() const override;
    void cleanup() override;

    int livesLeft() const { return _livesLeftProp.value(); }
    QBindable<int> bindableLivesLeft() const { return &_livesLeftProp; }
    int livesTotal() const { return _livesTotalProp.value(); }
    QBindable<int> bindableLivesTotal() const { return &_livesTotalProp; }

   public slots:
    void handleMessage(int id, Message msg);
    void setControlState(int objectId, TankControlState* controlState);
    void exitGame();

   signals:
    void sendMessage(Message msg);
    void attachToObject(int id);
    void deadRejoin();
    void gameOver(bool winner);
};

class AppContext;

class EditorGameState : public BaseGameState {
    Q_OBJECT
    QML_ELEMENT
    Q_MOC_INCLUDE("AppContext.hpp")
    Q_PROPERTY(AppContext* context READ context WRITE setContext BINDABLE bindableContext)
    Q_OBJECT_BINDABLE_PROPERTY(EditorGameState, AppContext*, _contextProp)

    std::map<int, std::shared_ptr<BaseObjectState>> _objectStates;
    QPropertyNotifier _contextNotifier;
    b2DynamicTree _queryTree;
    struct ObjectTreeData {
        int id;
        b2AABB realAABB;
        int32 proxyId;
    };
    std::map<int, ObjectTreeData> _objectTreeData;
    int _nextId = 1;
    void _removeObject(int id);

   public:
    EditorGameState(AppContext* context=nullptr);

    AppContext* context() const;
    void setContext(AppContext* context);
    QBindable<AppContext*> bindableContext() const;

    const std::map<int, std::shared_ptr<BaseObjectState>>& snapshot() const override;
    Q_INVOKABLE void clear();
    Q_INVOKABLE int addObject(int type, float x, float y, float rotation);
    Q_INVOKABLE void removeObjects(float x1, float y1, float x2, float y2);
    Q_INVOKABLE void save(QUrl fname) const;
    Q_INVOKABLE static EditorGameState* load(QUrl fname, AppContext* context);

    friend struct RemoveObjectsCallback;
};

#endif  // GAME_STATE_HPP
