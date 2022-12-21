import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtQuick.Shapes 1.2
import BT 1.0


Page {
    property alias state: map.state
    property alias controlledObjectId: map.controlledObjectId
    property alias controlledObject: map.controlledObject
    id: view

    Image {
        source: "qrc:/data/images/tiles.png"
        anchors.fill: parent
        smooth: false
        fillMode: Image.Tile
        transform: [
            Scale { xScale: 2; yScale: 2},
            Translate { x: map.viewCenter && (map.positionToPixels(Qt.point(0, 0)).x - width) % 48 - 48; y: map.viewCenter && (map.positionToPixels(Qt.point(0, 0)).y - height) % 48 - 48}
        ]
    }

    MapView {
        id: map
        anchors.fill: parent
    }

    header: ToolBar {
        RowLayout {
            spacing: 8
            anchors.fill: parent

            Column {
                spacing: 4

                Label {
                    height: 16
                    text: "Health"
                    anchors.right: parent.right
                }
                Label {
                    height: 16
                    text: "Shield"
                    anchors.right: parent.right
                }
            }

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

            Label {
                text: "Team lives"
                leftPadding: 20
            }

            Row {
                spacing: 2

                Repeater {
                    model: view.state.livesTotal
                    Image {
                        source: index >= view.state.livesLeft ? "qrc:/data/images/life_used.png" : "qrc:/data/images/life.png"
                        smooth: false
                        width: 20
                        height: 20
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
                    Rectangle {
                        height: moduleImg.height
                        width: moduleImg.width
                        color: "white"
                        opacity: 0.75
                        visible: modelData.reload == 0
                    }
                    Shape {
                        width: moduleImg.width
                        height: moduleImg.height
                        opacity: 0.5
                        layer.enabled: true
                        visible: modelData.reload > 0
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
                }
            }
        }
    }

    Keys.forwardTo: [map]
}
