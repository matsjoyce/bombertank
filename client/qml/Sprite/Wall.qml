import QtQuick 2.15
import BT

BaseSprite {
    id: base

    Image {
        source: base.MapView.object.health > 0.5 ? "qrc:/data/images/wall.png" : "qrc:/data/images/damaged_wall.png"
        x: -36
        y: -36
        width: 72
        height: 72
        smooth: false
    }
}
