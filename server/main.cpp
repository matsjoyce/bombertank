#include <QCoreApplication>

#include "GameServer.hpp"
#include "docopt.h"

static const char USAGE[] =
    R"(BT Server

    Usage:
      bt_server [--quit-when-no-clients]
      bt_server (-h | --help)
      bt_server --version

    Options:
      -h --help               Show this screen.
      --version               Show version.
      --quit-when-no-clients  Shutdown the server when all of the clients have disconnected.
)";

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    std::map<std::string, docopt::value> args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "BT Server 2.0a");

    GameServer gs(QHostAddress::Any, 3000);

    if (args["--quit-when-no-clients"].asBool()) {
        QObject::connect(&gs, &GameServer::lastClientDisconnected, &app, &QCoreApplication::quit);
    }

    return app.exec();
}