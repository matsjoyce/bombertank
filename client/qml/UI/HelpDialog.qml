import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects

Dialog {
    title: "Controls cheatsheet"
    standardButtons: Dialog.Ok
    modal: true
    horizontalPadding: 60

    GridLayout {
        rows: 2
        flow: GridLayout.TopToBottom
        columnSpacing: 60
        rowSpacing: 20

        Label {
            text: qsTr("Movement")
            font.bold: true
            font.pixelSize: 18
        }

        GridLayout {
            Layout.alignment: Qt.AlignTop

            Item {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                Layout.row: 2
                Layout.column: 2

                Image {
                    id: baseImage
                    source: "qrc:/data/images/tank_base.png"
                    anchors.fill: parent
                    smooth: false
                }

                Item {
                    id: tracksImages
                    property double yOffset: 0
                    x: baseImage.x
                    y: baseImage.y
                    width: 48
                    height: 48
                    clip: true
                    NumberAnimation { target: tracksImages; property: "yOffset"; from: 4; to: 0; duration: 300; loops: Animation.Infinite; running: true }

                    Image {
                        source: "qrc:/data/images/tank_track_marks.png"
                        y: tracksImages.yOffset * 2
                        width: 48
                        height: 48
                        smooth: false
                    }
                    Image {
                        source: "qrc:/data/images/tank_track_marks.png"
                        y: tracksImages.yOffset * 2
                        width: 48
                        height: 48
                        smooth: false
                        mirror: true
                    }
                }

                Image {
                    id: overlay
                    source: "qrc:/data/images/tank_color.png"
                    anchors.fill: parent
                    smooth: false
                    visible: false
                }

                Colorize {
                    id: color
                    anchors.fill: parent
                    source: overlay
                    hue: 0
                    lightness: -0.5
                }

                Image {
                    id: turretImage
                    source: "qrc:/data/images/tank_turret.png"
                    anchors.fill: parent
                    smooth: false
                    rotation: 60;
                    SequentialAnimation {
                        loops: Animation.Infinite; running: true
                        NumberAnimation { target: turretImage; property: "rotation"; from: 60; to: -60; duration: 1000 }
                        PauseAnimation { duration: 5000 }
                        NumberAnimation { target: turretImage; property: "rotation"; from: -60; to: 60; duration: 1000 }
                        PauseAnimation { duration: 5000 }
                    }
                }
            }
            Image {
                source: "qrc:/data/icons/rotate.svg"
                width: 30
                height: 40
                Layout.row: 2
                Layout.column: 1
                Layout.alignment: Qt.AlignVCenter
            }
            Image {
                source: "qrc:/data/icons/rotate.svg"
                width: 30
                height: 40
                mirror: true
                Layout.row: 2
                Layout.column: 3
                Layout.alignment: Qt.AlignVCenter
            }
            Image {
                source: "qrc:/data/icons/forward.svg"
                width: 30
                height: 30
                Layout.row: 1
                Layout.column: 2
                Layout.alignment: Qt.AlignHCenter
            }
            Image {
                source: "qrc:/data/icons/forward.svg"
                width: 30
                height: 30
                mirrorVertically: true
                Layout.row: 3
                Layout.column: 2
                Layout.alignment: Qt.AlignHCenter
            }
            KeyStack {
                Layout.alignment: Qt.AlignHCenter
                keys: ["W"]//, "↑"]
                Layout.row: 0
                Layout.column: 1
                Layout.columnSpan: 3
            }
            KeyStack {
                Layout.alignment: Qt.AlignHCenter
                keys: ["S"]//, "↓"]
                Layout.row: 4
                Layout.column: 1
                Layout.columnSpan: 3
            }
            KeyStack {
                Layout.alignment: Qt.AlignVCenter
                keys: ["A"]//, "←"]
                Layout.row: 1
                Layout.column: 0
                Layout.rowSpan: 3
            }
            KeyStack {
                Layout.alignment: Qt.AlignVCenter
                keys: ["D"]//, "→"]
                Layout.row: 1
                Layout.column: 4
                Layout.rowSpan: 3
            }
            Label {
                text: "Aim turret using the mouse"
                Layout.row: 5
                Layout.column: 0
                Layout.columnSpan: 5
                Layout.topMargin: 20
            }
        }

        Label {
            text: qsTr("Actions")
            font.bold: true
            font.pixelSize: 18
        }

        GridLayout {
            columns: 5
            Layout.alignment: Qt.AlignTop
            rowSpacing: 20

            Repeater {
                model: ["images/shell_icon.png", "images/mg_icon.png", "images/pwall.png", "images/health_box.png", "images/health_box.png"]

                Item {
                    width: 52
                    height: 52
                    Layout.alignment: Qt.AlignHCenter

                    Rectangle {
                        anchors.fill: parent
                        color: "white"
                        border.color: "black"
                        border.width: 2
                    }
                    Image {
                        source: "qrc:/data/" + modelData
                        anchors.fill: parent
                        anchors.margins: 2
                        smooth: false
                    }
                }
            }

            KeyStack {
                keys: ["1", "Space", "LMB"]
                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            }

            KeyStack {
                keys: ["2", "E", "RMB"]
                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            }

            KeyStack {
                keys: ["3", "Q"]
                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            }

            KeyStack {
                keys: ["4"]
                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            }

            KeyStack {
                keys: ["5"]
                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            }
        }

    }
}
