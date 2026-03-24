import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Rectangle {
    id: control

    signal clicked

    property alias text: label.text
    property real padding: 10

    width : 42
    height : width

    radius: width / 2

    color: mouse_area.containsPress ?
               Material.highlightedRippleColor : "transparent"

    border.color: Material.dividerColor
    border.width: mouse_area.containsPress ? 1 : 0

    implicitHeight: Math.max(label.implicitHeight, label.implicitWidth)
    implicitWidth: implicitHeight


    Label {
        id: label
        font.family: "Font Awesome 7 Free"

        font.pointSize: 48

        fontSizeMode: Label.Fit

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        anchors.fill: parent
        anchors.margins: control.padding
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


