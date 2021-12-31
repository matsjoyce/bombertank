import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import BT 1.0
import "editorUtils.js" as EditorUtils


Item {
    required property EditorGameState state
    readonly property var gridSizes: [0.5, 1, 2, 3, 6, 9]
    property double gridSize: 9
    id: view

    Image {
        source: "qrc:/data/images/tiles.png"
        anchors.fill: view
        smooth: false
        fillMode: Image.Tile
        transform: [
            Scale { xScale: 2; yScale: 2}
            //Translate { x: controlledObject ? -controlledObject.x*8 % 48 : 0; y: controlledObject ? controlledObject.y*8 % 48 : 0}
        ]
    }

    MapView {
        id: map
        state: view.state
        anchors.fill: view
    }

    MouseArea {
        id: mapMouseArea
        anchors.fill: map
        hoverEnabled: true
        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                dragRect.startDragPoint = dragRect.endDragPoint = Qt.point(mouse.x, mouse.y);
            }
        }
        onPositionChanged: {
            dragRect.endDragPoint = Qt.point(mouse.x, mouse.y);
            if (!(mouse.buttons & Qt.LeftButton)) {
                dragRect.startDragPoint = dragRect.endDragPoint;
            }
        }
        onReleased: {
            if (mouse.button == Qt.LeftButton) {
                dragRect.endDragPoint = Qt.point(mouse.x, mouse.y);

                for (var x = dragRect.dragRectPoints.startX; x <= dragRect.dragRectPoints.endX; ++x) {
                    for (var y = dragRect.dragRectPoints.startY; y <= dragRect.dragRectPoints.endY; ++y) {
                        view.state.addObject(0, x * view.gridSize, y * view.gridSize);
                    }
                }

                dragRect.startDragPoint = dragRect.endDragPoint;
            }
        }

        Rectangle {
            property point startDragPoint: "0,0"
            property point endDragPoint: "0,0"
            property var dragRectPoints: EditorUtils.rectFromDragPoints(startDragPoint, endDragPoint, view.gridSize, map)

            id: dragRect
            x: dragRectPoints.pixelStart.x
            y: dragRectPoints.pixelStart.y
            width: dragRectPoints.pixelEnd.x - dragRectPoints.pixelStart.x
            height: dragRectPoints.pixelEnd.y - dragRectPoints.pixelStart.y
            visible: true
            border.color: "red"
            border.width: 2
            color: mapMouseArea.pressed ? "#55ff0000" : "transparent"
        }
    }

    Rectangle {
        anchors.left: view.left
        anchors.top: view.top
        anchors.right: view.right
        height: topBar.height + 4
        width: 100
        color: "black"
        opacity: 0.6
    }

    RowLayout {
        id: topBar
        spacing: 4
        anchors.left: view.left
        anchors.top: view.top
        anchors.right: view.right
        anchors.margins: 2


        Button {
            text: "Open"
            enabled: false
        }

        Button {
            text: "Save"
            enabled: false
        }

        Button {
            text: "Clear"
            onClicked: view.state.clear()
        }

        Item {
            Layout.fillWidth: true
        }

        Text {
            text: "Grid size: %1".arg(view.gridSize)
            verticalAlignment: Text.AlignVCenter
            Layout.fillHeight: true
            Layout.minimumWidth: 80
            color: "white"
        }

        Slider {
            from: 0
            to: view.gridSizes.length - 1
            stepSize: 1
            value: view.gridSizes.indexOf(view.gridSize)

            onMoved: view.gridSize = view.gridSizes[value]
        }
    }

    Rectangle {
        anchors.fill: bottomBar
        color: "black"
        opacity: 0.6
    }

    Row {
        id: bottomBar
        padding: 2
        anchors.bottom: view.bottom
        width: view.width

        ListView {
            id: objectList
            model: [1, 2, 3]
            clip: true
            orientation: Qt.Horizontal
            spacing: 2
            height: 76
            width: view.width - 4

            delegate: Rectangle {
                height: 76
                width: 76
                border.width: 2
                border.color: ListView.isCurrentItem ? "red" : "white"

                Image {
                    source: "qrc:/data/images/wall.png"
                    x: 2
                    y: 2
                    width: 72
                    height: 72
                    smooth: false
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        objectList.currentIndex = index;
                    }
                }
            }
        }
    }
}
