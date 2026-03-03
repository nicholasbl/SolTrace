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
    property alias mouseArea: mouseArea
    property int cursorShape: Qt.ArrowCursor
    property real iconOpacity: 1
    property color labelColor: "white"
    property int labelSize: 14
    property int iconSize: 20
    property string labelFont: ""
    property int labelWeight: Font.Normal
    property bool underline: false

    signal clicked()

    implicitWidth: contentRow.implicitWidth + margins * 2
    implicitHeight: contentRow.implicitHeight + margins * 2
    width: implicitWidth
    height: implicitHeight

    Behavior on scale {
        NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
    }

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
        spacing: root.source !== "" && root.label !== "" ? 6 : 0

        Image {
            id: icon
            source: root.source
            width: root.iconSize
            height: root.iconSize
            anchors.verticalCenter: parent.verticalCenter
            visible: root.source !== ""
            opacity: root.iconOpacity
            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectFit
            smooth: true
            cache: true
        }

        Text {
            id: labelText
            text: root.label
            color: root.labelColor
            font.pointSize: root.labelSize
            font.weight: root.labelWeight
            font.family: root.labelFont || ""
            anchors.verticalCenter: parent.verticalCenter
            visible: root.label !== ""
            font.underline: root.underline
        }
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
