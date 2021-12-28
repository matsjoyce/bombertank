import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import "UI"

ApplicationWindow {
    minimumHeight: 640
    minimumWidth: 480
    visible: true
    visibility: Window.Maximized
    title: qsTr("Hello World")

    UI {
        anchors.fill: parent

        onClose: { close() }
    }
}
