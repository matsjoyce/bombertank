#ifndef OBJECT_TYPE_DATA_HPP
#define OBJECT_TYPE_DATA_HPP

#include <QString>
#include <QObject>
#include <QQmlEngine>


struct ClientOTD {
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

    Q_PROPERTY(QString image MEMBER image CONSTANT)
    Q_PROPERTY(bool editorPlacable MEMBER editorPlacable CONSTANT)
    Q_PROPERTY(QString renderer MEMBER renderer CONSTANT)

public:
    QString image;
    bool editorPlacable;
    QString renderer;
};

struct ServerOTD {
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

public:
    QString impl;
};

struct ObjectTypeData {
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

    Q_PROPERTY(int id MEMBER id CONSTANT)
    Q_PROPERTY(QString name MEMBER name CONSTANT)
    Q_PROPERTY(ClientOTD client MEMBER client CONSTANT)
    Q_PROPERTY(ServerOTD server MEMBER server CONSTANT)

public:
    int id;
    QString name;
    ClientOTD client;
    ServerOTD server;
};

std::vector<ObjectTypeData> loadObjectTypeData(QString fname);

struct TankModuleData {
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

    Q_PROPERTY(int id MEMBER id CONSTANT)
    Q_PROPERTY(QString name MEMBER name CONSTANT)
    Q_PROPERTY(QString image MEMBER image CONSTANT)
    Q_PROPERTY(std::vector<int> forSlots MEMBER forSlots CONSTANT)

public:
    int id;
    QString name;
    QString image;
    std::vector<int> forSlots;
};

std::vector<TankModuleData> loadTankModuleData(QString fname);

# endif // OBJECT_TYPE_DATA_HPP
