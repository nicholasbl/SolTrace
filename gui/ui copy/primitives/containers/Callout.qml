import QtQuick
import QtQuick.Layouts
import SolTraceProto

Item {
    id: root

    property bool hasContent: false
    property bool collapsed: hasContent ? false : true
    property string title: "Title"
    property int titleSize: 14
    property string titleFont: ""
    property int titleWeight: Font.Normal
    property int bodySize: 14
    property string bodyFont: titleFont
    property color titleColor: Theme.textColor
    property color bodyColor: Theme.textColor
    property real bodyOpacity: 0.8
    property int spacing: 8
    property list<Component> titleActions: []

    default property alias content: body.data

    signal clicked()
    signal collapseAction()

    width: parent.width
    implicitHeight: titleContainer.height + (root.collapsed ? 0 : bodyContainer.height)
    height: implicitHeight

    Component.onCompleted: {
        root.hasContent = root.content.length > 0
        if (root.hasContent) {
            set_chevron_icon()
        } else {
            set_dot_icon()
        }
    }

    function set_chevron_icon() {
        titleButton.source = collapsed ? "qrc:/icons/assets/icons/chevron-right.svg" : "qrc:/icons/assets/icons/chevron-down.svg"
    }

    function set_dot_icon() {
        titleButton.source = "qrc:/icons/assets/icons/dot.svg";
    }

    Rectangle {
        id: titleContainer
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width
        height: titleButton.implicitHeight
        color: "transparent"

        RowLayout {
            width: parent.width

            IconButton {
                id: titleButton
                source: ""
                Layout.preferredWidth: implicitWidth
                Layout.alignment: Qt.AlignVCenter
                onClicked: {
                    root.collapsed = root.hasContent ? !root.collapsed : true
                    if (root.hasContent) root.set_chevron_icon()
                }
            }

            IconButton {
                Layout.preferredWidth: implicitWidth
                Layout.alignment: Qt.AlignVCenter
                label: root.title
                labelSize: root.titleSize
                labelFont: root.titleFont
                labelWeight: root.titleWeight
                onClicked: root.clicked()
            }

            Item {
                Layout.fillWidth: true
            }

            Row {
                Layout.preferredWidth: implicitWidth
                Layout.alignment: Qt.AlignVCenter
                spacing: 5
                Repeater {
                    model: root.titleActions
                    delegate: Loader {
                        required property QtObject modelData
                        sourceComponent: modelData.component || modelData
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
