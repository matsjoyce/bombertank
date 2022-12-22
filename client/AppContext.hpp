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

    Q_PROPERTY(std::vector<ObjectTypeData> objectTypeDatas READ objectTypeDataValues CONSTANT)
    Q_PROPERTY(std::vector<TankModuleData> tankModuleDatas READ tankModuleDataValues CONSTANT)

    std::string _serverExePath;
    QProcess _localServerProc;
    QTcpSocket _conn;
    std::map<int, ObjectTypeData> _objectTypeData;
    std::map<int, TankModuleData> _tankModuleData;

    void handleLocalServerStarted();
    void handleLocalServerError();
    void handleServerConnection();
    void handleConnectionError();
    void connectToHost(QHostAddress address);

   public:
    AppContext(std::string serverExePath);

    const std::map<int, ObjectTypeData>& objectTypeDatas() const;
    std::vector<ObjectTypeData> objectTypeDataValues() const;
    Q_INVOKABLE ObjectTypeData objectTypeData(int type) const;

    const std::map<int, TankModuleData>& tankModuleDatas() const;
    std::vector<TankModuleData> tankModuleDataValues() const;
    Q_INVOKABLE std::vector<TankModuleData> tankModuleDatasForSlot(int slot) const;
    Q_INVOKABLE TankModuleData tankModuleData(int type) const;

   public slots:
    void connectToServer(QString address);
    void startLocalServer();

   signals:
    void connectedToServer(GameServer* server);
    void errorConnectingToServer(QString error);
    void errorStartingLocalServer(QString error);
};

#endif  // APP_CONTEXT_HPP
