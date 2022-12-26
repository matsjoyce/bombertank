#include <QCoreApplication>
#include <QCommandLineParser>

#include "GameServer.hpp"
#include "common/VCS.hpp"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("BomberTank2 Server");
    QCoreApplication::setApplicationVersion(GIT_NAME);


    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions(
        {
            {
                "quit-when-no-clients",
                QCoreApplication::translate("main", "Shutdown the server when all of the clients have disconnected."),
            }
        }
    );
    parser.process(app);

    GameServer gs(QHostAddress::Any, 3000);

    if (parser.isSet("quit-when-no-clients")) {
        QObject::connect(&gs, &GameServer::lastClientDisconnected, &app, &QCoreApplication::quit);
    }

    return app.exec();
}
