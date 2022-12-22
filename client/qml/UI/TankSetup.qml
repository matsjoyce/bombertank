import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import BT 1.0

Dialog {
    id: dialog
    title: "Tank setup"
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    property int selectedSlot: 0
    property var itemsForSlots: Array(5).fill(-1).map((_, i) => context.tankModuleDatasForSlot(i)[0].id)

    ColumnLayout {
        RowLayout {
            width: dialog.width

            Repeater {
                model: 5
                Rectangle {
                    height: 76
                    width: 76
                    border.width: 2
                    border.color: dialog.selectedSlot == index ? palette.highlight : palette.base

                    Image {
                        source: print(context.tankModuleData(itemsForSlots[index]), itemsForSlots[index]) || itemsForSlots[index] == -1 ? "" : ("qrc:/data/" + context.tankModuleData(itemsForSlots[index]).image)
                        x: 2
                        y: 2
                        width: 72
                        height: 72
                        smooth: false
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            dialog.selectedSlot = index;
                        }
                    }
                }
            }
        }

        Label {
            text: "Selected slot %1".arg(dialog.selectedSlot + 1)
        }

        ListView {
            id: moduleList
            model: [null, ...context.tankModuleDatasForSlot(dialog.selectedSlot)]
            clip: true
            spacing: 2
            Layout.minimumHeight: 76 * 3
            Layout.fillWidth: true
            currentIndex: Math.max(model.findIndex(i => i && i.id == dialog.itemsForSlots[dialog.selectedSlot]), 0)
            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AlwaysOn
            }

            delegate: ItemDelegate {
                id: delegate
                contentItem: Row {
                    padding: 2
                    spacing: 10
                    anchors.fill: parent
                    Rectangle {
                        width: 72
                        height: 72
                        color: "white"

                        Image {
                            source: modelData == null ? "" : "qrc:/data/" + modelData.image
                            smooth: false
                            anchors.fill: parent
                        }
                    }
                    Label {
                        text: modelData == null ? "Nothing" : modelData.name
                        color: delegate.highlighted ? palette.highlightedText : palette.text
                        height: delegate.height
                        verticalAlignment: Qt.AlignVCenter
                    }
                }
                width: moduleList.width
                height: 76
                highlighted: ListView.isCurrentItem

                onClicked: {
                    dialog.itemsForSlots = [
                        ...dialog.itemsForSlots.slice(0, dialog.selectedSlot),
                        modelData == null ? -1 : modelData.id,
                        ...dialog.itemsForSlots.slice(dialog.selectedSlot + 1)
                    ]
                }
            }
        }
    }
}
