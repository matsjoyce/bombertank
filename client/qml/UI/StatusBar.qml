import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Item {
    id: bar

    required property string barImage
    property double value: 0

    height: 16

    Rectangle {
        color: "black"
        anchors.fill: bar
    }

    Image {
        source: "qrc:/data/images/bar_bg.png"
        x: 2
        y: 2
        width: (bar.width / 2 - 2)
        height: 6
        smooth: false
        fillMode: Image.TileHorizontally
        horizontalAlignment: Image.AlignLeft
        transformOrigin: Item.TopLeft
        scale: 2.0
    }

    Image {
        source: bar.barImage
        x: 2
        y: 2
        width: (bar.width / 2 - 2) * Math.max(0, Math.min(1, bar.value))
        height: 6
        smooth: false
        fillMode: Image.TileHorizontally
        horizontalAlignment: Image.AlignLeft
        transformOrigin: Item.TopLeft
        scale: 2
    }

    Text {
        text: "%1%".arg(Math.round(bar.value * 100) || 0)
        anchors.centerIn: parent
        color: "white"
    }
}
