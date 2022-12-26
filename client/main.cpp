#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QDir>
#include <QCommandLineParser>
#include <iostream>

#include "AppContext.hpp"
#include "GameServer.hpp"
#include "GameState.hpp"
#include "MapView.hpp"
#include "common/TcpMessageSocket.hpp"
#include "common/VCS.hpp"
#include "objects/TankControl.hpp"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QQuickStyle::setStyle("Theme");
    QApplication app(argc, argv);
    QApplication::setApplicationName("BomberTank2 Client");
    QApplication::setApplicationVersion(GIT_NAME);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions(
        {
            {
                "server-exe",
                QCoreApplication::translate("main", "Path to the server executable (for local games and hosting)."),
                QCoreApplication::translate("main", "path"),
                ""
            }
        }
    );
    parser.process(app);

    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-Black.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-Bold.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-ExtraBold.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-Medium.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-Regular.ttf");
    QFontDatabase::addApplicationFont(":/data/fonts/Orbitron-SemiBold.ttf");

    QDir selfDir(argc >= 1 ? argv[0] : "");
    selfDir.cdUp();
    auto serverExePath = parser.value("server-exe").isEmpty() ? selfDir.filePath("bt_server") : parser.value("server-exe");
    qDebug() << "Server path" << serverExePath;

    AppContext appContext(serverExePath.toStdString());

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
