import QtQuick
import QtQuick.Particles

Item {
    property alias running: particles.enabled

    ParticleSystem {
        id: sys
    }

    ItemParticle {
        system: sys
        delegate: Rectangle {
            rotation: 45
            width: 2
            height: 2
            color: "yellow"
        }
    }

    Friction {
        factor: 3
        system: sys
    }

    Emitter {
        system: sys
        anchors.centerIn: parent
        id: particles
        emitRate: 40
        lifeSpan: 400
        velocity: AngleDirection {angleVariation: 360; magnitude: 60; magnitudeVariation: 30}
    }
}
