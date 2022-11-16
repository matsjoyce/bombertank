import QtQuick 2.15
import BT 1.0

Item {
    readonly property TankControlState controls: TankControlState {}
    property var interestingKeys: [Qt.Key_W, Qt.Key_S, Qt.Key_A, Qt.Key_D, Qt.Key_Space, Qt.Key_1, Qt.Key_2, Qt.Key_3, Qt.Key_4, Qt.Key_5]
    property var keys: {
        let initialKeys = {};
        interestingKeys.forEach(k => { initialKeys[k] = false });
        return initialKeys;
    }

    anchors.fill: parent

    function updateControls() {
        var lr = keys[Qt.Key_A] - keys[Qt.Key_D];
        var ud = keys[Qt.Key_W] - keys[Qt.Key_S];
        var lra = Math.PI/2 + lr * Math.PI/2;
        var uda = ud * Math.PI/2;
        controls.angle = (lr && ud) ? (lra + uda/2 * -lr) : (lr ? lra : uda);
        controls.turretAngle = -Math.atan2(mouseArea.mouseY - height/2, mouseArea.mouseX - width/2);
        controls.power = !!lr || !!ud;
        controls.setAction(0, keys[Qt.Key_Space] || keys[Qt.Key_1]);
        controls.setAction(1, keys[Qt.Key_2]);
        controls.setAction(2, keys[Qt.Key_3]);
        controls.setAction(3, keys[Qt.Key_4]);
        controls.setAction(4, keys[Qt.Key_5]);
    }

    Timer {
        interval: 1000 / 20
        repeat: true
        running: true

        onTriggered: updateControls()
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Keys.onPressed: {
        if (event.modifiers != Qt.NoModifier || event.isAutoRepeat) {
            return;
        }
        if (interestingKeys.includes(event.key)) {
            keys[event.key] = true;
            event.accepted = true;
        }
    }

    Keys.onReleased: {
        if (event.isAutoRepeat) {
            return;
        }
        if (interestingKeys.includes(event.key)) {
            keys[event.key] = false;
            event.accepted = true;
        }
    }
}
