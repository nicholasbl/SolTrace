import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Rectangle {
    id: control

    signal clicked

    property alias text: label.text
    property alias label: label
    property real padding: 10

    width: label.width + padding
    height: width

    radius: width / 2

    color: mouse_area.containsPress ?
               Material.highlightedRippleColor : "transparent"

    border.color: Material.dividerColor
    border.width: mouse_area.containsPress ? 1 : 0

    implicitHeight: Math.max(label.implicitHeight, label.implicitWidth) + padding
    implicitWidth: implicitHeight


    Label {
        id: label
        font.family: "Font Awesome 7 Free"

        font.pointSize: 16

        anchors.centerIn: parent
    }

    // Rectangle {
    //     anchors.fill: parent
    //     radius: width / 2
    // }

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        onClicked: control.clicked()
    }

}


