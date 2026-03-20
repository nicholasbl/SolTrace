import QtQuick
import SolTraceProto

MouseArea {
    id: root
    property bool checked: false
    property int radius: 25
    width: 25
    height: width

    onClicked: {
        checked = !checked
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.width: 1
        border.color: Theme.lineColor
        radius: root.radius

        Rectangle {
            anchors.fill: parent
            anchors.margins: 5
            color: root.checked ? Theme.textColor : "transparent"
            radius: root.radius * 0.5
        }
    }
}


