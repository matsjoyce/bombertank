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
            Translate { x: map.viewCenter && (map.positionToPixels(Qt.point(0, 0)).x - width) % 48 - 48; y: map.viewCenter && (map.positionToPixels(Qt.point(0, 0)).y - height) % 48 - 48}
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
                value: controlledObject ? controlledObject.shield : 0;
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
        id: bottomBg
        width: view.width
        height: 60
        y: view.height - 60
        color: "black"
        opacity: 0.6
    }

    Row {
        anchors.fill: bottomBg
        spacing: 4
        padding: 2
        Repeater {
            model: controlledObject ? controlledObject.modules : [];
            Rectangle {
                width: 50
                height: 50
                color: "grey"

                Rectangle {
                    height: 50
                    width: 50 * modelData.reload
                    color: "white"
                }
                Text {
                    anchors.fill: parent
                    text: "%1: %2".arg(index).arg(modelData.type)
                }
            }
        }
    }

    Keys.forwardTo: [map]
}
