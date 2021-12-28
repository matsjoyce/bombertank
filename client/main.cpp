#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <iostream>

#include "AppContext.hpp"
#include "GameServer.hpp"
#include "GameState.hpp"
#include "MapView.hpp"
#include "common/TcpMessageSocket.hpp"
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
      --server-exe=<-path>    Path to the server executable (for local games and hosting).
)";

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    qmlRegisterUncreatableType<AppContext>("BT", 1, 0, "AppContext", "Uncreatable!");
    qmlRegisterUncreatableType<GameServer>("BT", 1, 0, "GameServer", "Uncreatable!");
    qmlRegisterUncreatableType<ListedGame>("BT", 1, 0, "ListedGame", "Uncreatable!");
    qmlRegisterUncreatableType<GameState>("BT", 1, 0, "GameState", "Uncreatable!");
    qmlRegisterType<MapView>("BT", 1, 0, "MapView");
    qmlRegisterType<TankControlState>("BT", 1, 0, "TankControlState");
    qmlRegisterUncreatableMetaObject(constants::staticMetaObject, "BT", 1, 0, "Constants", "Uncreatable!");

    std::map<std::string, docopt::value> args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "BT Client 2.0a");

    QQuickStyle::setStyle("Theme");
    QApplication app(argc, argv);

    // int id = QFontDatabase::addApplicationFont(":/PixelEmulator-xq08.ttf");
    // QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    // QFont monospace(family);
    // for (auto ps : QFontDatabase::pointSizes(monospace.family(), "")) {
    //     std::cout << ps << std::endl;
    // }
    // //    monospace.setLetterSpacing(QFont::AbsoluteSpacing, -2);

    // monospace.setStyleStrategy(QFont::NoSubpixelAntialias);
    // monospace.setHintingPreference(QFont::PreferNoHinting);
    // monospace.setPointSize(16);
    // app.setFont(monospace);

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/qml/");

    AppContext appContext(args["--server-exe"].asString());
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
