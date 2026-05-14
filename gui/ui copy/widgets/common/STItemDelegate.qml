import QtQuick
import QtQuick.Controls.Basic
import SolTraceProto

// TODO: Merge with other controls
ItemDelegate {
    id: root

    contentItem: Label {
        rightPadding: root.spacing
        text: root.text
        font: root.font
        //anchors.fill: parent
        verticalAlignment: Qt.AlignVCenter
        elide: Label.ElideRight
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        opacity: root.enabled ? 1 : 0.3
        color: root.down ? Theme.buttonColor : "transparent"

        Rectangle {
            width: parent.width
            height: 1
            color: Theme.lineColor
            anchors.bottom: parent.bottom
        }
    }
}
