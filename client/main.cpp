#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QDir>
#include <iostream>

#include "AppContext.hpp"
#include "GameServer.hpp"
#include "GameState.hpp"
#include "MapView.hpp"
#include "common/TcpMessageSocket.hpp"
#include "common/VCS.hpp"
#include "objects/TankControl.hpp"
#include "docopt.h"

static const char USAGE[] =
    R"(BT Client

    Usage:
      bt_client [--server-exe=<path>]
      bt_client (-h | --help)
      bt_client --version

    Options:
      -h --help               Show this screen.
      --version               Show version.
      --server-exe=<path>    Path to the server executable (for local games and hosting).
)";

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    std::map<std::string, docopt::value> args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true, std::string{"BomberTank2 Client "} + GIT_NAME);

    QQuickStyle::setStyle("Theme");
    QApplication::setApplicationVersion(GIT_NAME);
    QApplication app(argc, argv);

    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-Black.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-Bold.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-ExtraBold.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-Medium.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-Regular.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-SemiBold.ttf");

    QDir selfDir(argc >= 1 ? argv[0] : "");
    selfDir.cdUp();
    auto serverExePath = args["--server-exe"].isString() ? args["--server-exe"].asString() : selfDir.filePath("bt_server").toStdString();
    qDebug() << "Server path" << QString::fromStdString(serverExePath);

    AppContext appContext(serverExePath);

    {
        QQmlApplicationEngine engine;
        engine.addImportPath("qrc:/qml/");
        engine.rootContext()->setContextProperty("context", &appContext);

        const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
        QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreated, &app,
            [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl) {
                    QCoreApplication::exit(-1);
                }
            },
            Qt::QueuedConnection);

        engine.load(url);
        return app.exec();
    }
}
