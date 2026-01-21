import QtQuick
import QtQuick.Controls.Material
import SolTraceProto

Item {
    id: root
    property string label: ""
    property double horizontalMargins: 16
    property double verticalMargins: 2
    property double radius: 8
    property color color: Theme.buttonColor
    property color borderColor: Theme.lineColor
    property color hoverColor: Theme.buttonHoverColor
    property color pressColor: "transparent"
    property bool hoverEnabled: true
    property string tooltip: ""
    property alias mouseArea: mouseArea
    property int cursorShape: Qt.ArrowCursor
    property color labelColor: "white"
    property int labelSize: 14
    property int iconSize: 20
    property var labelFont: ""
    property var labelWeight: Font.Normal
    property bool underline: false

    signal clicked()

    implicitWidth: labelText.implicitWidth + horizontalMargins * 2
    implicitHeight: labelText.implicitHeight + verticalMargins * 2
    width: implicitWidth
    height: implicitHeight

    Behavior on scale {
        NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: mouseArea.pressed ? root.pressColor : mouseArea.containsMouse ? root.hoverColor : root.color
        radius: root.radius
        border.color: root.borderColor
        border.width: 1

        Behavior on color {
            ColorAnimation { duration: 100 }
        }
    }

    Text {
        id: labelText
        text: root.label
        color: root.labelColor
        font.pointSize: root.labelSize
        font.weight: root.labelWeight
        font.family: root.labelFont || ""
        anchors.centerIn: parent
        visible: root.label !== ""
        font.underline: root.underline
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: parent.hoverEnabled
        cursorShape: root.cursorShape

        onEntered: root.scale = 1.01
        onExited: root.scale = 1.0
        onClicked: root.clicked()
    }

    Loader {
        active: root.tooltip !== ""
        sourceComponent: ToolTip {
            text: root.tooltip
            delay: 500
            visible: mouseArea.containsMouse
            parent: root
        }
    }
}
