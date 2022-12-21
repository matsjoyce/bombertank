import QtQuick 2.15
import QtQuick.Templates 2.15 as T

T.Button {
    id: control
    implicitHeight: 40
    implicitWidth: 200

    font.pointSize: 14

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: 1.0
        color: enabled ? (control.down || control.checked || control.hovered ? "white" : palette.dark) : "gray"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        opacity: 1.0
        border.color: enabled ? palette.dark : "gray"
        border.width: 2
        color: enabled && (control.down || control.checked || control.hovered) ? palette.dark : "white"
    }
}
