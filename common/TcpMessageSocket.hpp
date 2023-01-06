#ifndef TCP_MESSAGE_SOCKET_HPP
#define TCP_MESSAGE_SOCKET_HPP

#include <QByteArray>
#include <QTcpSocket>
#include "ToServer.pb.h"
#include "ToClient.pb.h"
#include <memory>

class BaseTcpMessageSocket : public QObject {
    Q_OBJECT

    QTcpSocket* _socket;
    QByteArray _recvBuffer;
    int _id;

    void readData();
    void handleDisconnected();

protected:
    virtual void parseBuffer(QByteArrayView buf) = 0;
    void sendBuffer(QByteArrayView buf);

   public:
    BaseTcpMessageSocket(QTcpSocket* socket, int id, QObject* parent);
    int id() const { return _id; }
    void close();
    void check() { readData(); }

   signals:
    void disconnected(int id);
};

class ToServerMessageSocket : public BaseTcpMessageSocket {
    Q_OBJECT
   public:
    using BaseTcpMessageSocket::BaseTcpMessageSocket;
    void parseBuffer(QByteArrayView buf) override;

   public slots:
    void sendMessage(std::shared_ptr<const bt_messages::ToServerMessage> message);

   signals:
    void messageRecieved(int id, std::shared_ptr<bt_messages::ToClientMessage> message);
};

class ToClientMessageSocket : public BaseTcpMessageSocket {
    Q_OBJECT
   public:
    using BaseTcpMessageSocket::BaseTcpMessageSocket;
    void parseBuffer(QByteArrayView buf) override;

   public slots:
    void sendMessage(std::shared_ptr<const bt_messages::ToClientMessage> message);

   signals:
    void messageRecieved(int id, std::shared_ptr<bt_messages::ToServerMessage> message);
};

#endif  // TCP_MESSAGE_SOCKET_HPP
