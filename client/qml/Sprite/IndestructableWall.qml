import QtQuick 2.15

BaseSprite {
    id: base

    Image {
        source: base.itemData && "qrc:/data/" + base.itemData.client.image
        x: -36
        y: -36
        width: 72
        height: 72
        smooth: false
        fillMode: Image.PreserveAspectFit
    }
}
