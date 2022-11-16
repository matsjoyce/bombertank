import QtQuick 2.15
import Qt5Compat.GraphicalEffects

BaseSprite {
    id: base

    Image {
        source: "qrc:/data/images/turret_base.png"
        x: -24
        y: -24
        width: 48
        height: 48
        smooth: false
    }

    Image {
        id: overlay
        source: "qrc:/data/images/mg_turret.png"
        x: -24
        y: -24
        width: 48
        height: 48
        smooth: false
        rotation: -(object.turretAngle || 0) / Math.PI * 180;
    }
}
