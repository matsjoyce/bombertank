import QtQuick 2.15
import BT.Common
import BT

Item {
    property bool removable: true

    visible: !(MapView.object.status & Constants.INVISIBLE) || MapView.object.controlled
    opacity: MapView.object.status & Constants.INVISIBLE ? 0.5 : 1
}
