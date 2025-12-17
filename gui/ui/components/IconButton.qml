import QtQuick
import QtQuick.Controls.Material

Item {
    id: root
    property string source: ""
    property string label: ""
    property double margins: 8
    property double radius: 0
    property color color: "transparent"
    property color hoverColor: "transparent"
    property color pressColor: "transparent"
    property bool hoverEnabled: true
    property string tooltip: ""
    property var mouseArea: mouseArea
    property int cursorShape: Qt.ArrowCursor
    property real iconOpacity: 1

    property color labelColor: "white"
    property int labelSize: 14
    property int iconSize: 20
    property var labelFont: ""
    property var labelWeight: Font.Normal

    signal clicked()

    implicitWidth: contentRow.implicitWidth + margins * 2
    implicitHeight: contentRow.implicitHeight + margins * 2
    width: implicitWidth
    height: implicitHeight

    Rectangle {
        id: background
        anchors.fill: parent
        color: mouseArea.pressed ? root.pressColor :
               mouseArea.containsMouse ? root.hoverColor :
               root.color
        radius: root.radius

        Behavior on color {
            ColorAnimation { duration: 100 }
        }
    }

    Row {
        id: contentRow
        anchors.centerIn: parent
        spacing: 8

        Image {
            id: icon
            source: root.source
            width: root.source !== "" ? root.iconSize : 0
            height: root.source !== "" ? root.iconSize : 0
            anchors.verticalCenter: parent.verticalCenter
            visible: root.source !== ""
            opacity: root.iconOpacity

            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectCrop
            smooth: true
        }

        Text {
            id: labelText
            text: root.label
            color: root.labelColor
            font.pointSize: root.labelSize
            font.weight: root.labelWeight
            font.family: root.labelFont ? root.labelFont : ""
            anchors.verticalCenter: parent.verticalCenter
            visible: root.label !== ""
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: parent.hoverEnabled
        cursorShape: root.cursorShape
        onEntered: parent.scale = 1.01
        onExited: parent.scale = 1.0
        onClicked: {
            root.clicked()
        }
    }

    ToolTip {
        text: root.tooltip
        delay: 500
        visible: root.tooltip !== "" && mouseArea.containsMouse
        parent: root
    }

    function setCursorShape(shape) {
        root.cursorShape = shape
    }
}
