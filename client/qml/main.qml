import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import "UI"

ApplicationWindow {
    id: window
    width: 640
    height: 480
    minimumHeight: 300
    minimumWidth: 300
    visible: true
    title: qsTr("Hello World")

    UI {
        anchors.fill: parent

        onClose: { close() }
    }
}
