import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import BT 1.0


Item {
    property alias state: map.state
    property alias controlledObjectId: map.controlledObjectId
    property alias controlledObject: map.controlledObject
    id: view

    MapView {
        id: map
        anchors.fill: view
    }

    Rectangle {
        width: view.width
        height: 40
        color: "black"
        opacity: 0.5
    }

    Column {
        padding: 2
        spacing: 4

        StatusBar {
            width: 400
            value: controlledObject ? controlledObject.health : 0;
            barImage: "qrc:/data/images/hp_bar_fg.png"
        }

        StatusBar {
            width: 400
            value: 0.25;
            barImage: "qrc:/data/images/shield_bar_fg.png"
        }
    }


    Rectangle {
        width: view.width
        height: 60
        y: view.height - 60
        color: "black"
        opacity: 0.5
    }

    Keys.forwardTo: [map]
}