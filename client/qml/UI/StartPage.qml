import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import BT 1.0

Item {
    id: page

    signal startGame(GameServer server)
    signal startEditor()
    signal exit()

    ColumnLayout {
        id: columnLayout
        width: 300
        height: 300
        x: 10
        anchors.verticalCenter: parent.verticalCenter

        Button {
            text: qsTr("Start local game")
            Layout.fillWidth: true
            onClicked: {
                context.startLocalServer()
                page.state = "CONNECTING";
            }
        }

        Button {
            text: qsTr("Start network game")
            Layout.fillWidth: true
            onClicked: {
                serverAddressDialog.open();
            }
        }

        Button {
            text: qsTr("Map editor")
            Layout.fillWidth: true
            onClicked: startEditor()
        }

        Button {
            text: qsTr("Exit")
            Layout.fillWidth: true
            onClicked: exit()
        }
    }

    Dialog {
        id: serverAddressDialog
        title: "Server address"
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok | Dialog.Cancel

        TextField {
            id: adddressInput
            selectByMouse: true
            placeholderText: "IP address (XXX.XXX.XXX.XXX)"
        }

        onAccepted: {
            serverAddressDialog.close();
            page.state = "CONNECTING";
            context.connectToServer(adddressInput.text);
            adddressInput.clear();
        }
        onRejected: {
            serverAddressDialog.close();
            adddressInput.clear();
        }
    }

    Item {
        id: connectingLayer
        anchors.fill: page
        visible: false

        Behavior on visible {
            PropertyAnimation {
                target: connectingLayerBackground
                property: "opacity"
                from: 0
                to: 0.75
                duration: 500
            }
        }

        Rectangle {
            id: connectingLayerBackground
            color: "black"
            opacity: 0
            anchors.fill: connectingLayer
        }

        Text {
            text: "Connecting..."
            color: "red"
            anchors.horizontalCenter: connectingLayer.horizontalCenter
            anchors.verticalCenter: connectingLayer.verticalCenter
        }
    }

    Item {
        id: failedLayer
        anchors.fill: page
        visible: false

        Behavior on visible {
            PropertyAnimation {
                target: failedLayerBackground
                property: "opacity"
                from: 0
                to: 0.75
                duration: 500
            }
        }

        Rectangle {
            id: failedLayerBackground
            color: "black"
            opacity: 0
            anchors.fill: failedLayer
        }

        ColumnLayout {
            anchors.horizontalCenter: failedLayer.horizontalCenter
            anchors.verticalCenter: failedLayer.verticalCenter

            Text {
                Layout.alignment: Qt.AlignCenter
                text: "Error"
                color: "red"
            }

            Button {
                text: "OK"
                onClicked: { page.state = "START" }
            }
        }
    }

    Connections {
        target: context
        function onConnectedToServer(server) {
            page.state = "START";
            startGame(server)
        }
        function onErrorConnectingToServer(msg) {
            page.state = "FAILED";
        }
        function onErrorStartingLocalServer(msg) {
            page.state = "FAILED";
        }
    }

    state: "START"
    states: [
        State {
            name: "START"
        },
        State {
            name: "CONNECTING"
            PropertyChanges { target: connectingLayer; visible: true }
        },
        State {
            name: "FAILED"
            PropertyChanges { target: failedLayer; visible: true }
        }
    ]
}
