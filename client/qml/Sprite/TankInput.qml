import QtQuick 2.15
import BT 1.0

Item {
    readonly property TankControlState controls: TankControlState {}
    property bool upKey: false
    property bool downKey: false
    property bool leftKey: false
    property bool rightKey: false
    property bool action0Key: false

    function updateControls() {
        var pi = 3.14;
        var lr = leftKey - rightKey;
        var ud = upKey - downKey;
        var lra = pi/2 + lr * pi/2;
        var uda = ud * pi/2;
        controls.angle = (lr && ud) ? (lra + uda/2 * -lr) : (lr ? lra : uda);
        controls.power = !!lr || !!ud;
        controls.setAction(0, action0Key);
    }

    Keys.onPressed: {
        if (event.modifiers != Qt.NoModifier || event.isAutoRepeat) {
            return;
        }
        if (event.key == Qt.Key_Up) {
            upKey = true;
        }
        else if (event.key == Qt.Key_Down) {
            downKey = true;
        }
        else if (event.key == Qt.Key_Left) {
            leftKey = true;
        }
        else if (event.key == Qt.Key_Right) {
            rightKey = true;
        }
        else if (event.key == Qt.Key_Space) {
            action0Key = true;
        }
        else {
            return;
        }
        event.accepted = true;
        updateControls();
    }

    Keys.onReleased: {
        if (event.isAutoRepeat) {
            return;
        }
        if (event.key == Qt.Key_Up) {
            upKey = false;
        }
        else if (event.key == Qt.Key_Down) {
            downKey = false;
        }
        else if (event.key == Qt.Key_Left) {
            leftKey = false;
        }
        else if (event.key == Qt.Key_Right) {
            rightKey = false;
        }
        else if (event.key == Qt.Key_Space) {
            action0Key = false;
        }
        else {
            return;
        }
        event.accepted = true;
        updateControls();
    }
}
