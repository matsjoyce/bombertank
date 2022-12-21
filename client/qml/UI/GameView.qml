import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtQuick.Shapes 1.2
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
        id: topBg
        width: view.width
        height: topBar.height + 8
        color: "black"
        opacity: 0.6
    }

    RowLayout {
        id: topBar
        spacing: 4
        x: 4
        y: 4
        width: view.width - 8

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

        Repeater {
            model: view.state.livesLeft
            Item {
                width: lifeImage.width + 4
                height: lifeImage.height + 4
                Image {
                    id: lifeImage
                    x: 2
                    y: 2
                    source: "qrc:/data/images/life.png"
                    scale: 2.0
                    smooth: false
                }
            }
        }

        Repeater {
            model: console.log(view.state.livesTotal - view.state.livesLeft) || view.state.livesTotal - view.state.livesLeft
            Item {
                width: lifeImage.width + 4
                height: lifeImage.height + 4
                Image {
                    id: lifeImage
                    x: 2
                    y: 2
                    source: "qrc:/data/images/life_used.png"
                    scale: 2.0
                    smooth: false
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }

        Repeater {
            model: controlledObject ? controlledObject.modules : [];
            Item {
                id: moduleImg
                width: 52
                height: 52

                Rectangle {
                    anchors.fill: parent
                    color: "white"
                    border.color: "black"
                    border.width: 2
                }
                Image {
                    source: modelData.type == -1 ? "" : "qrc:/data/" + context.tankModuleData[modelData.type].image
                    anchors.fill: parent
                    anchors.margins: 2
                    smooth: false
                }
                Shape {
                    width: moduleImg.width
                    height: moduleImg.height
                    opacity: 0.5
                    layer.enabled: true
                    ShapePath {
                        fillColor: "black"
                        strokeColor: "transparent"
                        startX: moduleImg.width / 2
                        startY: moduleImg.height / 2
                        PathAngleArc {
                            radiusX: moduleImg.width
                            radiusY: moduleImg.height
                            centerX: moduleImg.width / 2
                            centerY: moduleImg.height / 2
                            startAngle: -90
                            sweepAngle: -360 * (1-modelData.reload)
                        }
                        PathLine { x: moduleImg.width / 2; y: moduleImg.height / 2 }
                    }
                }
                Rectangle {
                    height: moduleImg.height
                    width: moduleImg.width
                    color: "black"
                    opacity: 0.5
                    visible: modelData.reload < 1.0
                }
            }
        }
    }

    Keys.forwardTo: [map]
}
