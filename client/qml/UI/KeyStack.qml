import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout {
    id: root
    property var keys: ["A", "B", "C", "D", "E"]

    Repeater {
        model: root.keys

        Column {
            width: root.width
            Layout.alignment: Qt.AlignHCenter
            Label {
                text: qsTr("or")
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignTop
                visible: index > 0
                width: root.width
                height: 25
            }
            Frame {
                // border.color: "#b81413"
                // border.width: 2
                // color: "white"
                anchors.horizontalCenter: parent.horizontalCenter

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
