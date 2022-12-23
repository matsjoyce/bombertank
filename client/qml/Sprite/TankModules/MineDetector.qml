import QtQuick

TankModule {
    Connections {
        target: module

        function onUsed() {
            flashAnim.restart();
        }
    }
    Repeater {
        model: module.points
        Image {
            source: "qrc:/data/images/mine_detected.png"
            x: -24 + (modelData.x - object.x) * 8
            y: -24 - (modelData.y - object.y) * 8
            width: 48
            height: 48
            smooth: false
        }
    }
    Rectangle {
        id: blueFlash
        anchors.centerIn: parent
        width: 8*25*2
        height: 8*25*2
        color: "transparent"
        border.color: "#01c9ff"
        border.width: 4
        radius: width/2
        visible: flashAnim.running

        NumberAnimation {
            id: flashAnim
            target: blueFlash
            property: "opacity"
            from: 1
            to: 0
            duration: 1000
        }
    }
}
