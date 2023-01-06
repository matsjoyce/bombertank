#include "TcpMessageSocket.hpp"
#include <sstream>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

// #define USE_TEXT_FORMAT

#ifdef USE_TEXT_FORMAT
#include <google/protobuf/text_format.h>
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
        parseBuffer(QByteArrayView(_recvBuffer).sliced(MESSAGE_SIZE_SIZE, size));
        _recvBuffer.remove(0, size + MESSAGE_SIZE_SIZE);
    }
}

void BaseTcpMessageSocket::handleDisconnected() {
    emit disconnected(_id);
}

void ToServerMessageSocket::parseBuffer(QByteArrayView buf) {
    auto msg = std::make_shared<bt_messages::ToClientMessage>();
#ifdef USE_TEXT_FORMAT
    google::protobuf::io::ArrayInputStream s(buf.constData(), buf.size());
    google::protobuf::TextFormat::Parse(&s, msg.get());
#else
    msg->ParseFromArray(buf.constData(), buf.size());
#endif
    emit messageRecieved(id(), msg);
}

void ToServerMessageSocket::sendMessage(std::shared_ptr<const bt_messages::ToServerMessage> message) {
#ifdef USE_TEXT_FORMAT
    std::string buf;
    google::protobuf::TextFormat::PrintToString(*message.get(), &buf);
    sendBuffer(buf);
#else
    sendBuffer(message->SerializeAsString());
#endif
}

void ToClientMessageSocket::parseBuffer(QByteArrayView buf) {
    auto msg = std::make_shared<bt_messages::ToServerMessage>();
#ifdef USE_TEXT_FORMAT
    google::protobuf::io::ArrayInputStream s(buf.constData(), buf.size());
    google::protobuf::TextFormat::Parse(&s, msg.get());
#else
    msg->ParseFromArray(buf.constData(), buf.size());
#endif
    emit messageRecieved(id(), msg);
}

void ToClientMessageSocket::sendMessage(std::shared_ptr<const bt_messages::ToClientMessage> message) {
#ifdef USE_TEXT_FORMAT
    std::string buf;
    google::protobuf::TextFormat::PrintToString(*message.get(), &buf);
    sendBuffer(buf);
#else
    sendBuffer(message->SerializeAsString());
#endif
}
