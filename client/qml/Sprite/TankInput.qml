import QtQuick 2.15
import BT 1.0

Item {
    readonly property TankControlState controls: TankControlState {}
    property bool forwardKey: false
    property bool backwardKey: false
    property bool leftKey: false
    property bool rightKey: false
    property bool action0Key: false

    function updateControls() {
        var lr = (leftKey ? -1 : 0) + (rightKey ? 1 : 0);
        var fb = (forwardKey ? 1 : 0) + (backwardKey ? -1 : 0);
        controls.leftTrack = fb == 0 ? lr : Math.max(-1, Math.min(1, lr * 0.5 + fb));
        controls.rightTrack = fb == 0 ? -lr : Math.max(-1, Math.min(1, -lr * 0.5 + fb));
        controls.setAction(0, action0Key);
    }

    Keys.onPressed: {
        if (event.modifiers != Qt.NoModifier || event.isAutoRepeat) {
            return;
        }
        if (event.key == Qt.Key_Up) {
            forwardKey = true;
        }
        else if (event.key == Qt.Key_Down) {
            backwardKey = true;
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
            forwardKey = false;
        }
        else if (event.key == Qt.Key_Down) {
            backwardKey = false;
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
