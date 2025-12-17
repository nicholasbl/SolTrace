import QtQuick

Item {
    id: root

    property bool hasContent: true
    property bool collapsed: hasContent ? false : true
    property string title: "Title"
    property int titleSize: 14
    property string titleFont: ""
    property var titleWeight: Font.Normal
    property int bodySize: 14
    property string bodyFont: titleFont
    property color titleColor: "white"
    property color bodyColor: "white"
    property real bodyOpacity: 0.8
    property int spacing: 8
    property var buttonGroups: []
    property list<Component> buttons: []

    default property alias content: body.data

    width: parent.width
    implicitHeight: titleContainer.height + (root.collapsed ? 0 : bodyContainer.height)
    height: implicitHeight

    Rectangle {
        id: titleContainer
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width
        height: titleButton.implicitHeight
        color: "transparent"

        IconButton {
            id: titleButton
            source: "qrc:/icons/assets/icons/chevron-" + (root.collapsed ? "right" : "down") + ".svg"
            onClicked: root.collapsed = root.hasContent ? !root.collapsed : true
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            label: root.title
            labelSize: root.titleSize
            labelFont: root.titleFont
            labelWeight: root.titleWeight
            iconOpacity: root.hasContent ? 1 : 0.45

            Row {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10
                spacing: 5

                Repeater {
                    model: root.buttons || []
                    delegate: Loader {
                        required property var modelData
                        sourceComponent: modelData
                        width: item ? item.implicitWidth : 0
                        height: item ? item.implicitHeight : 0
                    }
                }
            }
        }
    }

    Rectangle {
        id: bodyContainer
        width: parent.width
        height: body.implicitHeight
        anchors.top: titleContainer.bottom
        anchors.left: parent.left
        color: "transparent"
        visible: !root.collapsed

        Column {
            id: body
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: 20
            spacing: 0
        }
    }
}
