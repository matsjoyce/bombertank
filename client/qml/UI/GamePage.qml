import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtMultimedia
import BT 1.0

Item {
    id: page
    required property GameState state
    required property GameServer server

    signal exitGame()

    GameView {
        id: gameView
        anchors.fill: page
        state: page.state
    }

    Connections {
        target: state
        function onAttachToObject(id) {
            gameView.controlledObjectId = id;
            startSound.play();
        }
        function onDeadRejoin() {
            state.exitGame();
            tankSetupDialog.open();
        }
        function onGameOver(winner: bool) {
            tankSetupDialog.close();
            gameOverDialog.winner = winner;
            gameOverDialog.open();
        }
    }

    SoundEffect {
        id: startSound
        source: "qrc:/data/sounds/start.wav"
    }

    TankSetup {
        id: tankSetupDialog
        anchors.centerIn: parent
        onRejected: {
            tankSetupDialog.close();
            exitGame();
        }
        onAccepted: {
            tankSetupDialog.close();
            page.state = page.server.joinGame(page.state.id, tankSetupDialog.itemsForSlots);
        }
    }

    Dialog {
        id: gameOverDialog
        property bool winner: false
        anchors.centerIn: parent
        modal: true
        title: "Game over"
        standardButtons: Dialog.Ok
        onRejected: {
            gameOverDialog.close();
            state.exitGame();
            exitGame();
        }
        onAccepted: {
            gameOverDialog.close();
            state.exitGame();
            exitGame();
        }

        Label {
            text: gameOverDialog.winner ? "Your team wins!" : "Your team lost."
        }
    }

    Keys.onEscapePressed: {
        state.exitGame()
        exitGame()
    }
    Keys.forwardTo: [gameView]
    focus: true
}
