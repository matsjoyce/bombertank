import QtQuick 2.15
import QtMultimedia

BaseSprite {
    id: base
    removable: /*!sound.playing && */!anim.running

    // SoundEffect {
    //     id: sound
    //     source: "qrc:/data/sounds/bomb-2.wav"
    // }
    Component.onCompleted: {
        // sound.play();
        anim.start();
    }

    Rectangle {
        id: blueFlash
        anchors.centerIn: parent
        width: 8*30*2
        height: 8*30*2
        color: "#00fff8"
        radius: width/2

        NumberAnimation {
            id: anim
            target: blueFlash
            property: "opacity"
            from: 1
            to: 0
            duration: 500
        }
    }
}
