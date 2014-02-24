import QtQuick 2.0
import Ubuntu.Components 0.1

Image {
    id: dropshadow
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.top
    }
    source: Qt.resolvedUrl("../img/toolbar_dropshadow.png")
    Behavior on opacity {
        UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
    }
}
