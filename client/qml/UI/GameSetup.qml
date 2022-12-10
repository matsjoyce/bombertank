import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Qt.labs.platform 1.1 as Qlp
import BT 1.0

Dialog {
    id: dialog
    title: "Game setup"
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    property url selectedMap
    property alias gameTitle: gameTitleField.text

    onOpened: {
        dialog.selectedMap = "";
        dialog.gameTitle = "";
        mapOpenDialog.open();
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Label {
                text: "Map:"
            }
            TextField {
                Layout.fillWidth: true
                readOnly: true
                text: dialog.selectedMap.toString()
            }
            Button {
                text: "Select"
                onClicked: mapOpenDialog.open()
            }
        }
        RowLayout {
            Label {
                text: "Title:"
            }
            TextField {
                id: gameTitleField
                Layout.fillWidth: true
            }
        }
    }

    Qlp.FileDialog {
        id: mapOpenDialog
        nameFilters: ["BT Maps (*.btm2)"]
        onAccepted: {
            dialog.selectedMap = files[0]
            dialog.gameTitle = /.*[\/]([^\/.]+)[^\/]*$/.exec(files[0])[1]
        }
        onRejected: {
            if (dialog.selectedMap == "") {
                dialog.reject();
            }
        }
    }
}
