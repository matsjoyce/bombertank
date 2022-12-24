import QtQuick 2.15
import BT.Common

Item {
    property bool removable: true
//     required property var object

    visible: !(object.status & Constants.INVISIBLE) || object.controlled
    opacity: object.status & Constants.INVISIBLE ? 0.5 : 1
}
