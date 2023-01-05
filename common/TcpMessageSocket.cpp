#include "TcpMessageSocket.hpp"
#include <sstream>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

const auto READ_SIZE = 1024u;
using MessageSizeType = decltype(htonl(0));
const qsizetype MESSAGE_SIZE_SIZE = sizeof(MessageSizeType);

BaseTcpMessageSocket::BaseTcpMessageSocket(QTcpSocket* socket, int id, QObject* parent) : QObject(parent), _socket(socket), _id(id) {
    _socket->setSocketOption(QTcpSocket::LowDelayOption, 1);
    connect(_socket, &QTcpSocket::readyRead, this, &BaseTcpMessageSocket::readData);
    connect(_socket, &QTcpSocket::disconnected, this, &BaseTcpMessageSocket::handleDisconnected);
}

void BaseTcpMessageSocket::close() {
    _socket->disconnectFromHost();
    disconnect(_socket, &QTcpSocket::readyRead, this, &BaseTcpMessageSocket::readData);
    disconnect(_socket, &QTcpSocket::disconnected, this, &BaseTcpMessageSocket::handleDisconnected);
}


void TcpMessageSocket::sendMessage(const Message message) {
    std::stringstream buffer;
    msgpack::pack(buffer, message);
    auto data = buffer.str();
    sendBuffer(data);
}

void BaseTcpMessageSocket::sendBuffer(QByteArrayView buf) {
    auto offset = 0u;
    // FIXME Don't assume it works?
    auto nsize = htonl(buf.size());
    _socket->write(reinterpret_cast<const char*>(&nsize), MESSAGE_SIZE_SIZE);
    while (offset < buf.size()) {
        offset += _socket->write(buf.constData() + offset, buf.size() - offset);
    }
    _socket->flush();
}


void BaseTcpMessageSocket::readData() {
    QByteArray buf(READ_SIZE, 0);
    while (auto size = _socket->read(buf.data(), READ_SIZE)) {
        _recvBuffer.append(buf.constData(), size);
    }
    while (_recvBuffer.size() > MESSAGE_SIZE_SIZE) {
        auto size = ntohl(*reinterpret_cast<const MessageSizeType*>(_recvBuffer.constData()));
        if (_recvBuffer.size() - MESSAGE_SIZE_SIZE < size) {
            return;
        }
        parseBuffer(QByteArrayView(buf).sliced(MESSAGE_SIZE_SIZE, size));
        _recvBuffer.remove(0, size + MESSAGE_SIZE_SIZE);
    }
}

void TcpMessageSocket::parseBuffer(QByteArrayView buf) {
    msgpack::object_handle oh = msgpack::unpack(buf.constData(), buf.size());
    emit messageRecieved(id(), oh.get().as<Message>());
}


void BaseTcpMessageSocket::handleDisconnected() {
    emit disconnected(_id);
}

void ToServerMessageSocket::parseBuffer(QByteArrayView buf) {
    auto msg = std::make_shared<bt_messages::FromServer>();
    msg->ParseFromArray(buf.constData(), buf.size());
    emit messageRecieved(id(), msg);
}

void ToServerMessageSocket::sendMessage(std::shared_ptr<const bt_messages::ToServer> message) {
    sendBuffer(message->SerializeAsString());
}

void FromServerMessageSocket::parseBuffer(QByteArrayView buf) {
    auto msg = std::make_shared<bt_messages::ToServer>();
    msg->ParseFromArray(buf.constData(), buf.size());
    emit messageRecieved(id(), msg);
}

void FromServerMessageSocket::sendMessage(std::shared_ptr<const bt_messages::FromServer> message) {
    sendBuffer(message->SerializeAsString());
}
