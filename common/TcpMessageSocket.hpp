#ifndef TCP_MESSAGE_SOCKET_HPP
#define TCP_MESSAGE_SOCKET_HPP

#include <QByteArray>
#include <QTcpSocket>
#include <msgpack.hpp>

using Message = std::map<std::string, msgpack::type::variant>;

class TcpMessageSocket : public QObject {
    Q_OBJECT

    QTcpSocket* _socket;
    QByteArray _recvBuffer;
    int _id;

    void readData();
    void handleDisconnected();

   public:
    TcpMessageSocket(QTcpSocket* socket, int id, QObject* parent);
    int id() const { return _id; }

   public slots:
    void sendMessage(const Message message);

   signals:
    void messageRecieved(int id, Message message);
    void disconnected(int id);
};

#endif  // TCP_MESSAGE_SOCKET_HPP