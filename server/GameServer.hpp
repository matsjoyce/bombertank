#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <QTcpServer>

#include "common/TcpMessageSocket.hpp"

class GameServer;

class GameHandler : public QObject {
    Q_OBJECT

   public:
    GameHandler(GameServer* gs, std::vector<std::map<msgpack::type::variant, msgpack::type::variant>> startingObjects);
    void addConnection(int id);
    void removeConnection(int id);
    void sendMessage(int id, Message msg);

   signals:
    void _addConnection(int id);
    void _removeConnection(int id);
    void _sendMessage(int id, Message msg);
};

struct ConnectionInfo {
    TcpMessageSocket* connection;
    int game = -1;
};

class GameServer : public QObject {
    Q_OBJECT

    QTcpServer* _server;
    int _nextConnectionId = 1, _nextGameId = 1;
    std::map<int, ConnectionInfo> _connections;
    std::map<int, GameHandler*> _games;

   private slots:
    void handleConnection();
    void handleDisconnection(int id);
    void handleClientMessage(int id, Message msg);
    void handleGameMessage(int id, Message msg);

   public:
    GameServer(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);

    int addGame(const std::vector<std::map<msgpack::type::variant, msgpack::type::variant>>& startingObjects);

    friend class GameHandler;

   signals:
    void lastClientDisconnected();
};

#endif  // GAME_SERVER_HPP