import QtQuick 2.15
import QtMultimedia
import BT

BaseSprite {
    id: base
    removable: !fireSound.playing && !deathAnim.running

    SoundEffect {
        id: fireSound
        source: "qrc:/data/sounds/gun.wav"
    }
    Component.onCompleted: {
        fireSound.play();
    }

    Rectangle {
        x: -2
        y: -2
        width: 4
        height: 100
        radius: 2
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#ffbb00" }
            GradientStop { position: 1.0; color: "transparent" }
        }
        visible: !base.MapView.object.destroyed
    }
    Rectangle {
        id: hitCircle
        x: -radius
        y: -radius
        width: 2*radius
        height: 2*radius
        radius: 2
        color: "white"
        opacity: 1
        visible: base.MapView.object.destroyed

        ParallelAnimation {
            id: deathAnim
            running: base.MapView.object.destroyed
            NumberAnimation { target: hitCircle; property: "radius"; to: 20; duration: 500 }
            NumberAnimation { target: hitCircle; property: "opacity"; to: 0; duration: 500 }
        }
    }
}
