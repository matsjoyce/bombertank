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

TcpMessageSocket::TcpMessageSocket(QTcpSocket* socket, int id, QObject* parent) : QObject(parent), _socket(socket), _id(id) {
    _socket->setSocketOption(QTcpSocket::LowDelayOption, 1);
    connect(_socket, &QTcpSocket::readyRead, this, &TcpMessageSocket::readData);
    connect(_socket, &QTcpSocket::disconnected, this, &TcpMessageSocket::handleDisconnected);
}

void TcpMessageSocket::close() {
    _socket->disconnectFromHost();
    disconnect(_socket, &QTcpSocket::readyRead, this, &TcpMessageSocket::readData);
    disconnect(_socket, &QTcpSocket::disconnected, this, &TcpMessageSocket::handleDisconnected);
}


void TcpMessageSocket::sendMessage(const Message message) {
    std::stringstream buffer;
    msgpack::pack(buffer, message);
    auto data = buffer.str();
    auto offset = 0u;
    // FIXME Don't assume it works?
    auto nsize = htonl(data.size());
    _socket->write(reinterpret_cast<const char*>(&nsize), MESSAGE_SIZE_SIZE);
    while (offset < data.size()) {
        offset += _socket->write(data.data() + offset, data.size() - offset);
    }
    _socket->flush();
}

void TcpMessageSocket::readData() {
    QByteArray buf(READ_SIZE, 0);
    while (auto size = _socket->read(buf.data(), READ_SIZE)) {
        _recvBuffer.append(buf.constData(), size);
    }
    while (_recvBuffer.size() > MESSAGE_SIZE_SIZE) {
        auto size = ntohl(*reinterpret_cast<const MessageSizeType*>(_recvBuffer.constData()));
        if (_recvBuffer.size() - MESSAGE_SIZE_SIZE < size) {
            return;
        }

        msgpack::object_handle oh = msgpack::unpack(_recvBuffer.constData() + MESSAGE_SIZE_SIZE, size);

        emit messageRecieved(_id, oh.get().as<Message>());
        _recvBuffer.remove(0, size + MESSAGE_SIZE_SIZE);
    }
}

void TcpMessageSocket::handleDisconnected() {
    emit disconnected(_id);
}
