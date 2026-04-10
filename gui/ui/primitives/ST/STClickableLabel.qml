import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Label {
    id: control

    text: "Initialize"
    font.pointSize: 16

    signal clicked

    Rectangle {
        anchors.fill: parent
        radius: width / 2

        anchors.margins: -5

        color: mouse_area.containsPress ?
                   Material.highlightedRippleColor : "transparent"

        border.color: Material.dividerColor
        border.width: mouse_area.containsPress ? 1 : 0
    }

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        onClicked: control.clicked()
    }
}
