#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <QTcpServer>

#include "common/TcpMessageSocket.hpp"
#include "common/ObjectTypeData.hpp"

class GameServer;

class GameHandler : public QObject {
    Q_OBJECT

    std::string _title;

   public:
    GameHandler(GameServer* gs, const std::map<int, ObjectTypeData>& objectTypeData, const std::map<int, TankModuleData>& tankModuleData, std::vector<bt_messages::ToServerMessage_CreateGame_StartingObject> startingObjects, int id, std::string title);
    void addConnection(int id, std::shared_ptr<bt_messages::ToServerMessage> msg);
    void removeConnection(int id);
    void sendMessage(int id, std::shared_ptr<bt_messages::ToServerMessage> msg);
    std::string title() const { return _title; }

   signals:
    void _addConnection(int id, std::shared_ptr<bt_messages::ToServerMessage> msg);
    void _removeConnection(int id);
    void _sendMessage(int id, std::shared_ptr<bt_messages::ToServerMessage> msg);
    void gameOver(int gameId);
};

struct ConnectionInfo {
    ToClientMessageSocket* connection;
    int game = -1;
};

class GameServer : public QObject {
    Q_OBJECT

    QTcpServer* _server;
    int _nextConnectionId = 1, _nextGameId = 1;
    std::map<int, ConnectionInfo> _connections;
    std::map<int, GameHandler*> _games;

    std::map<int, ObjectTypeData> _objectTypeData;
    std::map<int, TankModuleData> _tankModuleData;

    void _sendStats();

   private slots:
    void handleConnection();
    void handleDisconnection(int id);
    void handleClientMessage(int id, std::shared_ptr<bt_messages::ToServerMessage> msg);
    void handleGameMessage(int id, std::shared_ptr<bt_messages::ToClientMessage> msg);
    void removeGame(int gameId);

   public:
    GameServer(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);

    int addGame(std::vector<bt_messages::ToServerMessage_CreateGame_StartingObject> startingObjects, std::string title);

    friend class GameHandler;

   signals:
    void lastClientDisconnected();
};

#endif  // GAME_SERVER_HPP
