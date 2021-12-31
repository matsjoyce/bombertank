import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import BT 1.0

Item {
    id: page
    property EditorGameState state: EditorGameState {}

    signal exit()

    EditorView {
        id: editorView
        anchors.fill: page
        state: page.state
    }

    Keys.onEscapePressed: {
        exit()
    }
    Keys.forwardTo: [editorView]
    focus: true
}

