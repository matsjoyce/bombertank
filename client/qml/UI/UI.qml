import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Item {
    signal close()

    id: ui
    implicitHeight: 640
    implicitWidth: 600
    state: "START"

    Loader {
        id: startPage
        source: "StartPage.qml"
        active: false

        visible: true
        anchors.fill: parent
    }
    Connections {
        target: startPage.item
        function onStartGame(server) {
            lobbyPage.setSource(lobbyPage.source, {server: server})
            ui.state = "LOBBY";
        }
        function onExit() { close() }
    }

    Loader {
        id: lobbyPage
        source: "LobbyPage.qml"
        active: false

        visible: true
        anchors.fill: parent
    }
    Connections {
        target: lobbyPage.item
        function onStartGame(server, gameState) {
            gamePage.setSource(gamePage.source, {server: server, state: gameState})
            ui.state = "GAME";

        }
    }

    Loader {
        id: gamePage
        source: "GamePage.qml"
        active: false

        visible: true
        anchors.fill: parent
    }
    Connections {
        target: gamePage.item
    }

    states: [
        State {
            name: "START"
            PropertyChanges { 
                target: startPage
                active: true
                focus: true
            }
        },
        State {
            name: "LOBBY"
            PropertyChanges {
                target: lobbyPage
                active: true
                focus: true
            }
        },
        State {
            name: "GAME"
            PropertyChanges {
                target: gamePage
                active: true
                focus: true
            }
        }
    ]
}
