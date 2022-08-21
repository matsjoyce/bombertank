import QtQuick 2.15
import Qt5Compat.GraphicalEffects

BaseSprite {
    id: base

    Image {
        source: "qrc:/data/images/tank1.png"
        x: -24
        y: -24
        width: 48
        height: 48
        smooth: false
    }

    Image {
        id: overlay
        source: "qrc:/data/images/tank_colored.png"
        x: -24
        y: -24
        width: 48
        height: 48
        smooth: false
        visible: false
    }

    Colorize {
        id: color
        anchors.fill: overlay
        source: overlay
        hue: 0.0
        lightness: -0.5
    }

    Timer {
        interval: 50
        running: true
        repeat: true
        onTriggered: {
            color.hue += 0.01
        }
    }
}
