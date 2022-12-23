#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "ObjectTypeData.hpp"


std::map<int, ObjectTypeData> loadObjectTypeData(QString fname) {
     std::map<int, ObjectTypeData> ret;
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
        auto data = ObjectTypeData{
            obj["id"].toInt(),
            obj["name"].toString(),
            {
                client["image"].toString(),
                client["editor_placable"].toBool(),
                client["renderer"].toString(),
                QSizeF{
                    client["editor_bounds"].toArray().at(0).toDouble(),
                    client["editor_bounds"].toArray().at(1).toDouble(),
                }
            },
            {
                "?"
            }
        };
        ret.emplace(std::make_pair(data.id, std::move(data)));
    }
    return ret;
}


std::map<int, TankModuleData> loadTankModuleData(QString fname) {
     std::map<int, TankModuleData> ret;
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
        auto data = TankModuleData{
            obj["id"].toInt(),
            obj["name"].toString(),
            obj["image"].toString(),
            obj["description"].toString(),
            obj["renderer"].toString(),
            forSlots
        };
        ret.emplace(std::make_pair(data.id, std::move(data)));
    }
    return ret;
}
