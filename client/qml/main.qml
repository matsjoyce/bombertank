import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import "UI"

ApplicationWindow {
    id: window
    minimumHeight: 640
    minimumWidth: 480
    visible: true
    visibility: Window.Maximized
    title: qsTr("BomberTank2")
    font.family: "Orbitron"
    font.pixelSize: 14

    palette {
        alternateBase: "grey"
        base: "white"
        brightText: "#b81413"
        button: "white"
        buttonText: "#b81413"
        dark: "#b81413"
        highlight: "#b81413"
        highlightedText: "white"
        light: "#b81413"
        link: "#b81413"
        linkVisited: "#b81413"
        mid: "#b81413"
        midlight: "white"
        shadow: "black"
        text: "black"
        toolTipBase: "grey"
        toolTipText: "black"
        window: "white"
        windowText: "black"
    }

    UI {
        anchors.fill: parent

        onClose: { window.close() }
    }
}
