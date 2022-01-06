#include "AppContext.hpp"

#include <QDebug>
#include <QFile>
#include <QHostAddress>
#include <QTimer>

AppContext::AppContext(std::string serverExePath) : _serverExePath(serverExePath), _objectTypeData(loadObjectTypeData(":/data/objects.json")) {
    _localServerProc.setProcessChannelMode(QProcess::ForwardedChannels);

    connect(&_localServerProc, &QProcess::started, this, &AppContext::handleLocalServerStarted);
    connect(&_localServerProc, &QProcess::errorOccurred, this, &AppContext::handleLocalServerError);
    connect(&_conn, &QTcpSocket::connected, this, &AppContext::handleServerConnection);
    connect(&_conn, &QTcpSocket::errorOccurred, this, &AppContext::handleConnectionError);
}

void AppContext::handleLocalServerStarted() { QTimer::singleShot(1000, this, &AppContext::connectToServer); }

void AppContext::handleLocalServerError() {
    qInfo() << "Error starting local server" << _localServerProc.errorString();
    emit errorStartingLocalServer(_localServerProc.errorString());
}

void AppContext::handleServerConnection() {
    qInfo() << "Connected to server";
    auto msgconn = new TcpMessageSocket(&_conn, 0, this);
    emit connectedToServer(new GameServer(msgconn, this));
}

void AppContext::handleConnectionError() {
    qInfo() << "Error connection to server" << _conn.errorString();
    emit errorConnectingToServer(_conn.errorString());
}

void AppContext::connectToServer() {
    qInfo() << "Connecting to server";
    if (_conn.state() != QTcpSocket::UnconnectedState) {
        _conn.disconnect();
    }
    _conn.connectToHost(QHostAddress::LocalHost, 3000);
}

void AppContext::startLocalServer() {
    if (_localServerProc.state() == QProcess::NotRunning) {
        if (QFile::exists(QString::fromStdString(_serverExePath))) {
            _localServerProc.start(QString::fromStdString(_serverExePath), QStringList() << "--quit-when-no-clients");
            return;
        }
        emit errorStartingLocalServer("Could not find server executable");
    }
    else {
        connectToServer();
    }
}
