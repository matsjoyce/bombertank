import QtQuick

TankModule {
    Connections {
        target: module

        function onUsed() {
            flashAnim.restart();
        }
    }
    Rectangle {
        id: blueFlash
        anchors.centerIn: parent
        width: 8*30*2
        height: 8*30*2
        color: "#00fff8"
        radius: width/2
        visible: flashAnim.running

        NumberAnimation {
            id: flashAnim
            target: blueFlash
            property: "opacity"
            from: 1
            to: 0
            duration: 500
        }
    }
}
