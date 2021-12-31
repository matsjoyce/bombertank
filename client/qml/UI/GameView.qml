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

    Image {
        source: "qrc:/data/images/tiles.png"
        anchors.fill: view
        smooth: false
        fillMode: Image.Tile
        transform: [
            Scale { xScale: 2; yScale: 2},
            Translate { x: controlledObject ? -controlledObject.x*8 % 48 : 0; y: controlledObject ? controlledObject.y*8 % 48 : 0}
        ]
    }

    MapView {
        id: map
        anchors.fill: view
    }

    Rectangle {
        width: view.width
        height: topBar.height
        color: "black"
        opacity: 0.6
    }

    Row {
        id: topBar
        spacing: 4
        padding: 2

        Column {
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

        Text {
            text: controlledObject ? "Side %1 Speed %2".arg(controlledObject.side).arg(controlledObject.speed) : ""
            color: "white"
            font.pixelSize: 20
            anchors.verticalCenter: parent.verticalCenter
        }
    }


    Rectangle {
        width: view.width
        height: 60
        y: view.height - 60
        color: "black"
        opacity: 0.6
    }

    Keys.forwardTo: [map]
}
