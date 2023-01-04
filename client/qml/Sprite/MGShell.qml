import QtQuick 2.15
import QtMultimedia
import BT

BaseSprite {
    id: base
    removable: !fireSound.playing && !deathAnim.running

    SoundEffect {
        id: fireSound
        source: "qrc:/data/sounds/small-gun.wav"
    }
    Component.onCompleted: {
        fireSound.play();
    }

    Rectangle {
        x: -1
        y: -1
        width: 2
        height: 50
        radius: 1
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#ffff00" }
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
            NumberAnimation { target: hitCircle; property: "radius"; to: 10; duration: 200 }
            NumberAnimation { target: hitCircle; property: "opacity"; to: 0; duration: 200 }
        }
    }
}

