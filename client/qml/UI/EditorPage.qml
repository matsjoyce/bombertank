import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Qt.labs.platform 1.1

import BT 1.0
import "editorUtils.js" as EditorUtils


Page {
    property EditorGameState state: EditorGameState {
        Component.onCompleted: {
            // Work around QTBUG-109597
            state.context = appContext;
        }
    }
    readonly property var gridSizes: [2, 3, 6, 9]
    property double gridSize: 9
    property url fname: ""
    property int selectedObjectType: (md => md !== null ? md.id : -1)(objectList.model[objectList.currentIndex])
    property AppContext appContext: context

    signal exit()

    id: view
    focus: true

    function moveView(x: int, y: int) {
        map.viewCenter = Qt.point(map.viewCenter.x + x, map.viewCenter.y + y);
        dragRect.startDragPoint = Qt.point(dragRect.startDragPoint.x + x, dragRect.startDragPoint.y + y);
        dragRect.endDragPoint = Qt.point(dragRect.endDragPoint.x + x, dragRect.endDragPoint.y + y);
    }

    Keys.onEscapePressed: {
        exit()
    }
    Keys.onLeftPressed: {
        moveView(-4, 0);
    }
    Keys.onRightPressed: {
        moveView(4, 0);
    }
    Keys.onUpPressed: {
        moveView(0, 4);
    }
    Keys.onDownPressed: {
        moveView(0, -4);
    }

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
        state: view.state
        anchors.fill: parent
        context: appContext
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

                if (selectedObjectType !== -1) {
                    for (var x = dragRect.dragRectPoints.startX; x <= dragRect.dragRectPoints.endX; ++x) {
                        for (var y = dragRect.dragRectPoints.startY; y <= dragRect.dragRectPoints.endY; ++y) {
                            view.state.addObject(selectedObjectType, x * view.gridSize, y * view.gridSize);
                        }
                    }
                }
                else {
                    view.state.removeObjects(
                        dragRect.dragRectPoints.startX * view.gridSize, dragRect.dragRectPoints.startY * view.gridSize,
                        dragRect.dragRectPoints.endX * view.gridSize, dragRect.dragRectPoints.endY * view.gridSize
                    )
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

    Canvas {
        id: gridCanvas
        anchors.fill: parent
        visible: showGridButton.checked

        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            ctx.lineWidth = 2;
            ctx.strokeStyle = "#66000000";

            var tlPos = map.positionToPixels(Qt.point(-view.gridSize / 2, -view.gridSize / 2));
            var brPos = map.positionToPixels(Qt.point(view.gridSize / 2, view.gridSize / 2));
            var xDiff = Math.abs(tlPos.x - brPos.x);
            var yDiff = Math.abs(tlPos.y - brPos.y);
            var xOffset = tlPos.x % xDiff;
            var yOffset = tlPos.y % yDiff;

            ctx.beginPath();
            var yn = height / yDiff + 1;
            for(var i = 0; i < yn; i++){
                ctx.moveTo(0, yDiff * i + yOffset);
                ctx.lineTo(width, yDiff * i + yOffset);
            }

            var xn = width / xDiff + 1;
            for(var i = 0; i < xn; i++){
                ctx.moveTo(xDiff * i + xOffset, 0);
                ctx.lineTo(xDiff * i + xOffset, height);
            }
            ctx.closePath();
            ctx.stroke();
        }

        Connections {
            target: map
            function onViewCenterChanged() { gridCanvas.requestPaint(); }
        }

        Connections {
            target: view
            function onGridSizeChanged() { gridCanvas.requestPaint(); }
        }
    }

    header: ToolBar {
        RowLayout {
            spacing: 4
            anchors.fill: parent

            Button {
                text: "Open"
                onClicked: mapOpenDialog.open()

                FileDialog {
                    id: mapOpenDialog
                    currentFile: fname
                    nameFilters: ["BT Maps (*.btm2)"]
                    onAccepted: {
                        view.fname = file;
                        // Static methods aren't really a thing in QML
                        view.state = view.state.load(file, context);
                        map.viewCenter = "0,0"
                    }
                }
            }

            Button {
                text: "Save"
                onClicked: mapSaveDialog.open()

                FileDialog {
                    id: mapSaveDialog
                    currentFile: fname
                    fileMode: FileDialog.SaveFile
                    nameFilters: ["BT Maps (*.btm2)"]
                    defaultSuffix: "btm2"
                    onAccepted: {
                        view.fname = file;
                        view.state.save(file);
                    }
                }
            }

            Button {
                text: "Clear"
                onClicked: {
                    view.state.clear()
                    map.viewCenter = "0,0"
                }
            }

            Button {
                text: "Exit"
                onClicked: exit()
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: "Grid"
                Layout.alignment: Qt.AlignVCenter
                font.pixelSize: 18
            }

            Switch {
                id: showGridButton
                checked: true
            }

            Label {
                text: "Size: %1".arg(view.gridSize)
                Layout.alignment: Qt.AlignVCenter
                Layout.minimumWidth: 60
            }

            Slider {
                from: 0
                to: view.gridSizes.length - 1
                stepSize: 1
                snapMode: Slider.SnapAlways
                value: view.gridSizes.indexOf(view.gridSize)
                focusPolicy: Qt.NoFocus

                onMoved: view.gridSize = view.gridSizes[value]
            }
        }
    }

    footer: ToolBar {
        Row {
            width: parent.width
            ListView {
                id: objectList
                model: [null, ...context.objectTypeDatas.filter(d => d.client.editorPlacable)]
                clip: true
                orientation: Qt.Horizontal
                spacing: 2
                height: 76
                width: parent.width
                currentIndex: 1

                delegate: Rectangle {
                    height: 76
                    width: 76
                    border.width: 2
                    border.color: ListView.isCurrentItem ? palette.highlight : palette.base
                    opacity: ListView.isCurrentItem ? 1.0 : 0.5

                    Image {
                        source: modelData !== null ? "qrc:/data/" + modelData.client.image : ""
                        x: 2
                        y: 2
                        width: 72
                        height: 72
                        smooth: false
                    }

                    Label {
                        anchors.centerIn: parent
                        text: "Delete"
                        visible: modelData === null
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
}
