import QtQuick 2.15
import QtMultimedia
import BT

BaseSprite {
    id: base

    SoundEffect {
        id: fireSound
        source: "qrc:/data/sounds/buzz.wav"
        loops: SoundEffect.Infinite
        volume: Math.min(1, base.MapView.object.length)
    }

    Rectangle {
        x: -1
        y: -base.MapView.object.length * 8
        width: 2
        height: base.MapView.object.length * 8
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "#fd4040" }
            GradientStop { position: 0.5; color: "#ff8282" }
            GradientStop { position: 1.0; color: "#fd4040" }
        }
        visible: base.MapView.object.length > 0
    }
}


