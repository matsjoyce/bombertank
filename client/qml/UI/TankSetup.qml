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
    property var itemsForSlots: Array(5).fill(-1).map((_, i) => context.tankModuleData.find(item => item.forSlots.includes(i)).id)

    ColumnLayout {
        RowLayout {
            width: dialog.width

            Repeater {
                model: 5
                Rectangle {
                    height: 76
                    width: 76
                    border.width: 2
                    border.color: dialog.selectedSlot == index ? "red" : "white"

                    Image {
                        source: itemsForSlots[index] == -1 ? "" : ("qrc:/data/" + context.tankModuleData.find(i => i.id == itemsForSlots[index]).image)
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

        Text {
            text: "Selected slot %1".arg(dialog.selectedSlot + 1)
        }

        ListView {
            id: moduleList
            model: {
                let model = [null];
                context.tankModuleData.map(item => {
                    if (item.forSlots.includes(dialog.selectedSlot)) {
                        model.push(item)
                    }
                });
                return model;
            }
            clip: true
            spacing: 2
            height: 76 * 3
            width: 200
            currentIndex: Math.max(model.findIndex(i => i && i.id == dialog.itemsForSlots[dialog.selectedSlot]), 0)

            delegate: Rectangle {
                height: 76
                width: moduleList.width
                border.width: 2
                border.color: ListView.isCurrentItem ? "red" : "white"

                Row {
                    padding: 2
                    spacing: 2
                    anchors.fill: parent
                    Image {
                        source: modelData == null ? "" : "qrc:/data/" + modelData.image
                        width: 72
                        height: 72
                        smooth: false
                    }
                    Text {
                        text: modelData == null ? "Nothing" : modelData.name
                    }
                }

                MouseArea {
                    anchors.fill: parent
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
}
