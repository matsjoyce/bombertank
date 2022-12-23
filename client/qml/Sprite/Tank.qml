import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import BT
import BT.Common

BaseSprite {
    id: base

    Image {
        source: "qrc:/data/images/tank_base.png"
        x: -24
        y: -24
        width: 48
        height: 48
        smooth: false
    }

    Image {
        id: overlay
        source: "qrc:/data/images/tank_color.png"
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
        hue: (object.side - 1) / 4
        lightness: -0.5
    }

    Image {
        source: "qrc:/data/images/tank_turret.png"
        x: -24
        y: -24
        width: 48
        height: 48
        smooth: false
        rotation: -(object.turretAngle - object.rotation || 0) / Math.PI * 180;
    }

    StunnedAnimation {
        running: object.status & Constants.STUNNED
    }

    Repeater {
        model: object ? object.modules : [];
        Loader {
            property TankModuleState module: modelData
            property var data: modelData.type == -1 ? null : context.tankModuleData(modelData.type)
            anchors.centerIn: base
            source: (modelData.type == -1 || data.renderer == "") ? "" : "qrc:/qml/Sprite/TankModules/" + data.renderer
        }
    }
}
