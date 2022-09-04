#ifndef GAME_HPP
#define GAME_HPP

#include <QObject>
#include <memory>
#include <vector>
#include <random>
#include <utility>
#include <optional>

#include "box2d/box2d.h"
#include "common/Constants.hpp"
#include "common/TcpMessageSocket.hpp"
#include "objects/Base.hpp"

class Game : public QObject, public b2ContactListener {
    Q_OBJECT

    std::vector<int> _connections;
    std::vector<std::pair<int, Message>> _messages;
    std::map<int, std::unique_ptr<BaseObjectState>> _objects;
    std::mt19937 _randomGen;
    int _nextId = 1;

    b2World _world;

   public:
    Game();
    void mainloop();
    std::optional<std::pair<int, BaseObjectState*>> addObject(constants::ObjectType type, b2Vec2 position, float rotation, b2Vec2 velocity);
    void PreSolve(b2Contact *contact, const b2Manifold* oldManifold) override;
    void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    BaseObjectState *object(int id);
    std::vector<int> objectsOnSide(int side);
    std::vector<int> objectsOfType(constants::ObjectType type);
    void attachPlayerToObject(int id, int objId);
    std::mt19937& randomGenerator() { return _randomGen; }
    const b2World* world() const { return &_world; }

   public slots:
    void addConnection(int id, Message msg);
    void removeConnection(int id);
    void recieveMessage(int id, Message msg);

   signals:
    void sendMessage(int id, Message msg);
    void playerConnected(int id, Message msg);
    void playerAttachedObjectDied(int id);
};

#endif  // GAME_HPP
