import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import BT 1.0

Item {
    id: page
    required property GameState state

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
        }
    }

    Keys.onEscapePressed: {
        state.exitGame()
        exitGame()
    }
    Keys.forwardTo: [gameView]
    focus: true
}
