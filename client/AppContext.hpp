#ifndef APP_CONTEXT_HPP
#define APP_CONTEXT_HPP

#include <QObject>
#include <QProcess>
#include <QTcpSocket>
#include <QQmlEngine>

#include "GameServer.hpp"
#include "common/ObjectTypeData.hpp"

class AppContext : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

    Q_PROPERTY(std::vector<ObjectTypeData> objectTypeData READ objectTypeData CONSTANT)
    Q_PROPERTY(std::vector<TankModuleData> tankModuleData READ tankModuleData CONSTANT)

    std::string _serverExePath;
    QProcess _localServerProc;
    QTcpSocket _conn;
    std::vector<ObjectTypeData> _objectTypeData;
    std::vector<TankModuleData> _tankModuleData;

    void handleLocalServerStarted();
    void handleLocalServerError();
    void handleServerConnection();
    void handleConnectionError();
    void connectToHost(QHostAddress address);

   public:
    AppContext(std::string serverExePath);
    const std::vector<ObjectTypeData>& objectTypeData() const { return _objectTypeData; }
    const std::vector<TankModuleData>& tankModuleData() const { return _tankModuleData; }

   public slots:
    void connectToServer(QString address);
    void startLocalServer();

   signals:
    void connectedToServer(GameServer* server);
    void errorConnectingToServer(QString error);
    void errorStartingLocalServer(QString error);
};

#endif  // APP_CONTEXT_HPP
