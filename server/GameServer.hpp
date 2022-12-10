#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <QTcpServer>

#include "common/TcpMessageSocket.hpp"

class GameServer;

class GameHandler : public QObject {
    Q_OBJECT

    std::string _title;

   public:
    GameHandler(GameServer* gs, std::vector<std::map<msgpack::type::variant, msgpack::type::variant>> startingObjects, int id, std::string title);
    void addConnection(int id, Message msg);
    void removeConnection(int id);
    void sendMessage(int id, Message msg);
    std::string title() const { return _title; }

   signals:
    void _addConnection(int id, Message msg);
    void _removeConnection(int id);
    void _sendMessage(int id, Message msg);
    void gameOver(int gameId);
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

    void _sendStats();

   private slots:
    void handleConnection();
    void handleDisconnection(int id);
    void handleClientMessage(int id, Message msg);
    void handleGameMessage(int id, Message msg);
    void removeGame(int gameId);

   public:
    GameServer(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);

    int addGame(const std::vector<std::map<msgpack::type::variant, msgpack::type::variant>>& startingObjects, std::string title);

    friend class GameHandler;

   signals:
    void lastClientDisconnected();
};

#endif  // GAME_SERVER_HPP
