import QtQuick 2.15
import QtMultimedia

BaseSprite {
    id: base

    SoundEffect {
        id: fireSound
        source: "qrc:/data/sounds/missile.wav"
    }
    Component.onCompleted: {
        fireSound.play();
    }

    Image {
        source: "qrc:/data/images/rocket.png"
        x: -6
        y: -6
        width: 12
        height: 36
        smooth: false
    }
}

