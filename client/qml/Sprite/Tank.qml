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
        hue: object.side / 4
        lightness: -0.5
    }
}
