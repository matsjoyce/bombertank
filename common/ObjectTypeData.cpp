#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "ObjectTypeData.hpp"


std::vector<ObjectTypeData> loadObjectTypeData(QString fname) {
     std::vector<ObjectTypeData> ret;
     QFile f(fname);

    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open type data file");
        return ret;
    }

    QJsonDocument loadDoc(QJsonDocument::fromJson(f.readAll()));

    for (auto item : loadDoc.array()) {
        auto obj = item.toObject();
        auto client = obj["client"].toObject();
        auto server = obj["server"].toObject();
        ret.emplace_back(ObjectTypeData{
            obj["id"].toInt(),
            obj["name"].toString(),
            {
                client["image"].toString(),
                client["editor_placable"].toBool(),
                client["renderer"].toString()
            },
            {
                "?"
            }
        });
    }
    return ret;
}


std::vector<TankModuleData> loadTankModuleData(QString fname) {
     std::vector<TankModuleData> ret;
     QFile f(fname);

    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open type data file");
        return ret;
    }

    QJsonDocument loadDoc(QJsonDocument::fromJson(f.readAll()));

    for (auto item : loadDoc.array()) {
        auto obj = item.toObject();
        std::vector<int> forSlots;
        for (auto slot : obj["for_slots"].toArray()) {
            forSlots.push_back(slot.toInt());
        }
        ret.emplace_back(TankModuleData{
            obj["id"].toInt(),
            obj["name"].toString(),
            obj["image"].toString(),
            forSlots
        });
    }
    return ret;
}
