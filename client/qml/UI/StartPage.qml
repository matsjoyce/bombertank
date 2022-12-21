import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import BT 1.0

Page {
    id: page

    signal startGame(GameServer server)
    signal startEditor()
    signal exit()

    header: Label {
        width: parent.width
        text: "BomberTank2"
        font.pixelSize: 70
        color: palette.highlight
        padding: 10
    }

    footer: Label {
        width: parent.width
        text: "Version %1".arg(Qt.application.version)
        color: palette.highlight
        padding: 10
        horizontalAlignment: Qt.AlignRight
    }

    ColumnLayout {
        spacing: 10
        x: 10
        anchors.verticalCenter: parent.verticalCenter
        width: 300

        Button {
            text: qsTr("Start local game")
            Layout.fillWidth: true
            onClicked: {
                context.startLocalServer()
                page.state = "CONNECTING";
            }
        }

        Button {
            text: qsTr("Join networked server")
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
            text: qsTr("Control cheatsheet")
            Layout.fillWidth: true
            onClicked: helpDialog.open()
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
            anchors.fill: parent
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

    Popup {
        id: connectingDialog
        modal: true
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose

        Label {
            text: "Connecting..."
        }
    }

    Dialog {
        id: failedDialog
        property string errorMessage: ""
        title: "Error"
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok
        onRejected: { page.state = "START" }
        onAccepted: { page.state = "START" }

        Label {
            Layout.alignment: Qt.AlignCenter
            text: failedDialog.errorMessage
        }
    }

    HelpDialog {
        id: helpDialog
        anchors.centerIn: parent
    }

    Connections {
        target: context
        function onConnectedToServer(server) {
            page.state = "START";
            startGame(server)
        }
        function onErrorConnectingToServer(msg) {
            page.state = "FAILED";
            failedDialog.errorMessage = msg;
        }
        function onErrorStartingLocalServer(msg) {
            page.state = "FAILED";
            failedDialog.errorMessage = msg;
        }
    }

    state: "START"
    states: [
        State {
            name: "START"
        },
        State {
            name: "CONNECTING"
            PropertyChanges { target: connectingDialog; visible: true }
        },
        State {
            name: "FAILED"
            PropertyChanges { target: failedDialog; visible: true }
        }
    ]
}
