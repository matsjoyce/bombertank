#ifndef GAME_HPP
#define GAME_HPP

#include <QObject>
#include <memory>
#include <vector>

#include "box2d/box2d.h"
#include "common/Constants.hpp"
#include "common/TcpMessageSocket.hpp"
#include "objects/Base.hpp"

class Game : public QObject, public b2ContactListener {
    Q_OBJECT

    std::vector<int> _connections;
    std::vector<std::pair<int, Message>> _messages;
    std::map<int, std::unique_ptr<BaseObjectState>> _objects;
    int _nextId = 1;

    b2World _world;

   public:
    Game();
    void mainloop();
    BaseObjectState *addObject(constants::ObjectType type, b2Vec2 position, float rotation, b2Vec2 velocity);
    void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    BaseObjectState *object(int id);
    std::vector<int> objectsOnTeam(int team);
    std::vector<int> objectsOfType(constants::ObjectType type);
    void attachPlayerToObject(int id, int objId);

   public slots:
    void addConnection(int id);
    void removeConnection(int id);
    void recieveMessage(int id, Message msg);

   signals:
    void sendMessage(int id, Message msg);
    void playerConnected(int id);
    void playerAttachedObjectDied(int id);
};

#endif  // GAME_HPP