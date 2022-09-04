import QtQuick 2.15
import QtMultimedia

BaseSprite {
    id: base
    removable: !sound.playing && !anim.running

    SoundEffect {
        id: sound
        source: "qrc:/data/sounds/bomb-2.wav"
    }
    Component.onCompleted: {
        sound.play();
        anim.start();
    }

    Rectangle {
        id: hitCircle
        x: -radius
        y: -radius
        width: 2*radius
        height: 2*radius
        radius: 8*15
        color: "#fcdd55"
        opacity: 1

        ParallelAnimation {
            id: anim
            NumberAnimation { target: hitCircle; property: "radius"; to: 12*15; duration: 100 }
            NumberAnimation { target: hitCircle; property: "opacity"; to: 0; duration: 500 }
        }
    }
}

