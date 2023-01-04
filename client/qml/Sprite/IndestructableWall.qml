import QtQuick 2.15
import BT

BaseSprite {
    id: base

    Image {
        source: "qrc:/data/" + base.MapView.objectData.image
        x: -36
        y: -36
        width: 72
        height: 72
        smooth: false
        fillMode: Image.PreserveAspectFit
    }
}
