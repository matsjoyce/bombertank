#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "ObjectTypeData.hpp"

std::variant<std::monostate, BoxGeometry, PolyGeometry> parseGeometry(QJsonValueRef data) {
    if (!data.isObject()) {
        return {};
    }
    auto obj = data.toObject();
    if (obj["type"].toString() == "box") {
        return {BoxGeometry{static_cast<float>(obj["width"].toDouble()), static_cast<float>(obj["height"].toDouble())}};
    }
    else if (obj["type"].toString() == "poly") {
        std::vector<std::pair<float, float>> points;
        for (auto point : obj["points"].toArray()) {
            auto pointAsArr = point.toArray();
            points.emplace_back(std::make_pair(
                static_cast<float>(pointAsArr.at(0).toDouble()),
                static_cast<float>(pointAsArr.at(1).toDouble())
            ));
        }
        return {PolyGeometry{points}};
    }
    return {};
}

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
                client["controller"].toString(),
                QSizeF{
                    client["editor_bounds"].toArray().at(0).toDouble(),
                    client["editor_bounds"].toArray().at(1).toDouble(),
                }
            },
            {
                server["impl"].toString(),
                parseGeometry(server["geometry"])
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
            obj["controller_overlay"].toString(),
            forSlots
        };
        ret.emplace(std::make_pair(data.id, std::move(data)));
    }
    return ret;
}
