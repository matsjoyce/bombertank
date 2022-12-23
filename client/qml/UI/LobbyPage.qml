import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import BT 1.0

Page {
    id: page
    required property GameServer server

    signal startGame(GameServer server, GameState gameState)
    signal leaveServer()

    GameSetup {
        id: gameSetupDialog
        anchors.centerIn: parent
        onAccepted: server.createGame(gameSetupDialog.selectedMap, gameSetupDialog.gameTitle)
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 4

            Button {
                text: "Join game"
                enabled: gameList.currentIndex >= 0

                onClicked: tankSetupDialog.open()
            }

            Button {
                text: "Create game"

                onClicked: gameSetupDialog.visible = true
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: "Leave server"

                onClicked: {
                    page.server.disconnect();
                    leaveServer();
                }
            }
        }
    }

        ListView {
            id: gameList
            anchors.fill: parent
            anchors.margins: 4
            model: page.server ? page.server.listedGamesModel : []
            clip: true

            delegate: ItemDelegate {
                required property int id
                required property string title
                required property int index

                text: "%1 %2".arg(id).arg(title)
                width: gameList.width
                highlighted: ListView.isCurrentItem

                onClicked: {
                    gameList.currentIndex = index;
                }
            }
        }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            Label {
                text: "Connected players: %1".arg(page.server.connectedCount)
                Layout.fillWidth: true
            }
            Label {
                text: "Server version: %1".arg(page.server.serverVersion)
            }
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
