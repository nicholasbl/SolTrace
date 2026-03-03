import QtQuick
import SolTraceProto

// TODO: Merge with other controls
Text {
    id: root
    color: Theme.textColor
    font.pointSize: Theme.textSize
    font.weight: Font.Bold
    font.family: "CMU Serif"

    signal clicked

    Rectangle {
        opacity: mouse_area.containsMouse ? 1 : 0
        radius: Theme.listItemRectRadius

        border.color: Theme.lineColor
        color: Theme.buttonHoverColor

        anchors.fill: parent

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }
    }

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked()
        }
    }
}
