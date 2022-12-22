#include "AppContext.hpp"

#include <QDebug>
#include <QFile>
#include <QHostAddress>
#include <QTimer>

AppContext::AppContext(std::string serverExePath) : _serverExePath(serverExePath), _objectTypeData(loadObjectTypeData(":/data/objects.json")), _tankModuleData(loadTankModuleData(":/data/tank_modules.json")) {
    _localServerProc.setProcessChannelMode(QProcess::ForwardedChannels);

    connect(&_localServerProc, &QProcess::started, this, &AppContext::handleLocalServerStarted);
    connect(&_localServerProc, &QProcess::errorOccurred, this, &AppContext::handleLocalServerError);
    connect(&_conn, &QTcpSocket::connected, this, &AppContext::handleServerConnection);
    connect(&_conn, &QTcpSocket::errorOccurred, this, &AppContext::handleConnectionError);
}

const std::map<int, ObjectTypeData>& AppContext::objectTypeDatas() const {
    return _objectTypeData;
}

std::vector<ObjectTypeData> AppContext::objectTypeDataValues() const {
    std::vector<ObjectTypeData> ret;
    ret.reserve(_objectTypeData.size());
    for (auto& [_, v] : _objectTypeData) {
        ret.emplace_back(v);
    }
    return ret;
}

ObjectTypeData AppContext::objectTypeData(int type) const {
    return _objectTypeData.at(type);
}

const std::map<int, TankModuleData>& AppContext::tankModuleDatas() const {
    return _tankModuleData;
}

std::vector<TankModuleData> AppContext::tankModuleDataValues() const {
    std::vector<TankModuleData> ret;
    ret.reserve(_tankModuleData.size());
    for (auto& [_, v] : _tankModuleData) {
        ret.emplace_back(v);
    }
    return ret;
}

std::vector<TankModuleData> AppContext::tankModuleDatasForSlot(int slot) const {
    std::vector<TankModuleData> ret;
    for (auto& [_, v] : _tankModuleData) {
        if (std::find_if(v.forSlots.begin(), v.forSlots.end(), [&](auto& s){ return s == slot; }) != v.forSlots.end()) {
            ret.emplace_back(v);
        }
    }
    return ret;
}

TankModuleData AppContext::tankModuleData(int type) const {
    return _tankModuleData.at(type);
}

void AppContext::handleLocalServerStarted() { QTimer::singleShot(1000, this, [this](){ connectToHost(QHostAddress::LocalHost); }); }

void AppContext::handleLocalServerError() {
    qInfo() << "Error starting local server" << _localServerProc.errorString();
    emit errorStartingLocalServer(_localServerProc.errorString());
}

void AppContext::handleServerConnection() {
    qInfo() << "Connected to server";
    auto msgconn = new TcpMessageSocket(&_conn, 0, this);
    emit connectedToServer(new GameServer(msgconn, this));
    msgconn->check();
}

void AppContext::handleConnectionError() {
    qInfo() << "Error connection to server" << _conn.errorString();
    emit errorConnectingToServer(_conn.errorString());
}

void AppContext::connectToServer(QString address) {
    connectToHost(QHostAddress(address));
}

void AppContext::connectToHost(QHostAddress address) {
    qInfo() << "Connecting to server" << address;
    if (_conn.state() != QTcpSocket::UnconnectedState) {
        _conn.disconnect();
    }
    _conn.connectToHost(address, 3000);
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
        connectToHost(QHostAddress::LocalHost);
    }
}
