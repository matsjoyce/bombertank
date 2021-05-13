import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import BT 1.0

Item {
    id: page
    required property GameServer server

    signal startGame(GameServer server, GameState gameState)

    GridLayout {
        anchors.fill: parent

        Button {
            text: "Join Game"

            onClicked: startGame(page.server, page.server.joinGame(gameList.currentItem.listedGame.id))
        }

        Button {
            text: "Create Game"

            onClicked: page.server.createGame()
        }

        ListView {
            id: gameList
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.minimumWidth: 100
            Layout.minimumHeight: 100
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 5
            model: page.server ? page.server.listedGamesModel : []
            clip: true

            delegate: Rectangle {
                required property ListedGame listedGame
                required property int index
                height: 40
                width: gameList.width
                color: ListView.isCurrentItem ? "red" : "white"

                Text {
                    text: listedGame.title
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
    }
}
