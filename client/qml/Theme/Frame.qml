import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

T.Frame {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    padding: 12

    background: Rectangle {
        color: "transparent"
        border.color: control.palette.dark
        border.width: 2
    }
}

