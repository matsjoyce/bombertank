import QtQuick 2.15
import QtQuick.Templates 2.15 as T
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15

T.Button {
    id: control
    implicitHeight: 40
    implicitWidth: 200

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.5
        color: enabled ? (control.down || control.checked ? "white" : control.hovered ? "red" : "black") : "gray"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        opacity: enabled ? 1.0 : 0.5
        border.color: enabled ? "red" : "gray"
        border.width: 2
        color: control.down || control.checked ? "red" : "white"
    }
}
