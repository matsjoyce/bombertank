import QtQuick 2.15
import BT 1.0

Item {
    readonly property TankControlState controls: TankControlState {}
    property var keys: { return {}; }
    property var interestingKeys: [Qt.Key_Up, Qt.Key_Down, Qt.Key_Left, Qt.Key_Right, Qt.Key_Space, Qt.Key_1, Qt.Key_2, Qt.Key_3, Qt.Key_4, Qt.Key_5]

    function updateControls() {
        var pi = 3.14;
        var lr = keys[Qt.Key_Left] - keys[Qt.Key_Right];
        var ud = keys[Qt.Key_Up] - keys[Qt.Key_Down];
        var lra = pi/2 + lr * pi/2;
        var uda = ud * pi/2;
        controls.angle = (lr && ud) ? (lra + uda/2 * -lr) : (lr ? lra : uda);
        controls.power = !!lr || !!ud;
        controls.setAction(0, keys[Qt.Key_Space] || keys[Qt.Key_1]);
        controls.setAction(1, keys[Qt.Key_2]);
        controls.setAction(2, keys[Qt.Key_3]);
        controls.setAction(3, keys[Qt.Key_4]);
        controls.setAction(4, keys[Qt.Key_5]);
    }

    Keys.onPressed: {
        if (event.modifiers != Qt.NoModifier || event.isAutoRepeat) {
            return;
        }
        if (interestingKeys.includes(event.key)) {
            keys[event.key] = true;
            event.accepted = true;
            updateControls();
        }
    }

    Keys.onReleased: {
        if (event.isAutoRepeat) {
            return;
        }
        if (interestingKeys.includes(event.key)) {
            keys[event.key] = false;
            event.accepted = true;
            updateControls();
        }
    }
}
