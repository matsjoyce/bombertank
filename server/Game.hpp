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
#include "common/ObjectTypeData.hpp"
#include "objects/Base.hpp"

class Game : public QObject, public b2ContactListener {
    Q_OBJECT

    std::vector<int> _connections;
    std::vector<std::pair<int, std::shared_ptr<bt_messages::ToServerMessage>>> _messages;
    std::map<int, std::unique_ptr<BaseObjectState>> _objects;
    std::map<int, int> _objToAttachedPlayer;
    std::map<int, int> _playerToAttachedObj;
    std::map<int, std::shared_ptr<bt_messages::ToClientMessage>> _previousObjectMsg;
    std::mt19937 _randomGen;
    int _nextId = 1;
    bool _active = true;

    const std::map<int, ObjectTypeData>& _objectTypeData;
    const std::map<int, TankModuleData>& _tankModuleData;

    b2World _world;

   public:
    Game(const std::map<int, ObjectTypeData>& objectTypeData, const std::map<int, TankModuleData>& tankModuleData);
    void mainloop();
    std::optional<std::pair<int, BaseObjectState*>> addObject(constants::ObjectType type, b2Vec2 position, float rotation, b2Vec2 velocity);
    void PreSolve(b2Contact *contact, const b2Manifold* oldManifold) override;
    void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;
    BaseObjectState *object(int id);
    std::vector<int> objectsOnSide(int side) const;
    std::vector<int> objectsOfType(int type) const;
    void attachPlayerToObject(int id, int objId);
    std::optional<int> attachedObjectForPlayer(int id) const;
    std::mt19937& randomGenerator() { return _randomGen; }
    const b2World* world() const { return &_world; }
    float timestep() const;
    const ServerOTD& dataForType(int type) const;

   public slots:
    void addConnection(int id, std::shared_ptr<bt_messages::ToServerMessage> msg);
    void removeConnection(int id);
    void recieveMessage(int id, std::shared_ptr<bt_messages::ToServerMessage> msg);
    void end();

   signals:
    void sendMessage(int id, std::shared_ptr<bt_messages::ToClientMessage> msg);
    void playerConnected(int id, std::shared_ptr<bt_messages::ToServerMessage> msg);
    void playerAttachedObjectDied(int id);
};

#endif  // GAME_HPP
