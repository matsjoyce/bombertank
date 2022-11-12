import QtQuick 2.15
import QtMultimedia

BaseSprite {
    id: base

    SoundEffect {
        id: fireSound
        source: "qrc:/data/sounds/buzz.wav"
        loops: SoundEffect.Infinite
    }

    Connections {
        target: object

        function onLengthChanged(value: double) {
            if (value > 0 && !fireSound.playing) {
                fireSound.play();
            }
            else if (value == 0 && fireSound.playing) {
                fireSound.stop();
            }
        }
    }

    Rectangle {
        x: -1
        y: -object.length * 8
        width: 2
        height: object.length * 8
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "#ff0000" }
            GradientStop { position: 0.5; color: "#ffaaaa" }
            GradientStop { position: 1.0; color: "#ff0000" }
        }
        visible: object.length > 0
    }
}


