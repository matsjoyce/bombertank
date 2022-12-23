import QtQuick 2.15
import BT.Common

Item {
    property bool removable: true
//     required property var object

    visible: !(object.status & Constants.INVISIBLE)
}
