import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout {
    id: root
    property var keys: ["A", "B", "C", "D", "E"]

    Repeater {
        model: root.keys
        Column {
            Label {
                text: qsTr("or")
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignTop
                visible: index > 0
                width: parent.width
                height: 25
            }
            Rectangle {
                border.color: "red"
                border.width: 2
                color: "white"
                height: 30
                width: 50

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    text: modelData
                }
            }
        }
    }
}
