import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Qt.labs.platform 1.1
import BT 1.0

Item {
    id: page
    required property GameServer server

    signal startGame(GameServer server, GameState gameState)
    signal leaveServer()

    Component.onCompleted: {
        console.log(page.server);
    }

    FileDialog {
        id: mapOpenDialog
        nameFilters: ["BT Maps (*.btm2)"]
        onAccepted: server.createGame(files[0])
    }

    GridLayout {
        anchors.fill: parent

        Button {
            text: "Join game"
            enabled: gameList.currentIndex >= 0

            onClicked: tankSetupDialog.open()
        }

        Button {
            text: "Create game"

            onClicked: mapOpenDialog.visible = true
        }

        Button {
            text: "Leave server"

            onClicked: {
                page.server.disconnect();
                leaveServer();
            }
        }

        ListView {
            id: gameList
            Layout.row: 1
            Layout.columnSpan: 3
            Layout.minimumWidth: 100
            Layout.minimumHeight: 100
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 5
            model: page.server ? page.server.listedGamesModel : []
            clip: true

            delegate: Rectangle {
                required property int id
                required property string title
                required property int index
                height: 40
                width: gameList.width
                color: ListView.isCurrentItem ? "red" : "white"

                Text {
                    x: 2
                    y: 2
                    text: "%1 %2".arg(id).arg(title)
                    color: ListView.isCurrentItem ? "white" : "black"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        gameList.currentIndex = index;
                    }
                }
            }
        }

        Text {
            Layout.row: 2
            Layout.columnSpan: 3
            padding: 2
            text: "Connected players: %1".arg(page.server.connectedCount)
        }
    }

    TankSetup {
        id: tankSetupDialog
        anchors.centerIn: parent
        onRejected: tankSetupDialog.close()
        onAccepted: {
            tankSetupDialog.close();
            startGame(page.server, page.server.joinGame(gameList.currentItem.id, tankSetupDialog.itemsForSlots));
        }
    }
}
