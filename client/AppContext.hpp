#ifndef APP_CONTEXT_HPP
#define APP_CONTEXT_HPP

#include <QObject>
#include <QProcess>
#include <QTcpSocket>

#include "GameServer.hpp"

class AppContext : public QObject {
    Q_OBJECT

    QProcess _localServerProc;
    QTcpSocket _conn;

    void handleLocalServerStarted();
    void handleLocalServerError();
    void handleServerConnection();
    void handleConnectionError();

   public:
    AppContext();

   public slots:
    void connectToServer();
    void startLocalServer();

   signals:
    void connectedToServer(GameServer* server);
    void errorConnectingToServer(QString error);
    void errorStartingLocalServer(QString error);
};

#endif  // APP_CONTEXT_HPP