import QtQuick 2.15
import QtQuick.Layouts 2.15
import SolTraceProto

Item {
    id: root
    property real minTabWidth: 50
    property int index: 0
    property list<string> model: ["First", "Second", "Third", "Fourth", "Fifth"]

    signal indexRequested(int newIndex)

    width: parent.width
    height: tabs.height + topDivider.height + bottomDivider.height

    Rectangle {
        id: topDivider
        width: parent.width
        height: 1
        color: Theme.lineColor
        anchors.top: parent.top
    }

    RowLayout {
        id: tabs
        spacing: 5
        width: parent.width
        anchors.top: topDivider.bottom

        Repeater {
            id: tabRepeater
            model: root.model
            delegate: Column {
                id: tabItem
                required property int index
                required property string modelData

                Layout.preferredWidth: Math.max(tabButton.implicitWidth, root.minTabWidth)
                Layout.fillHeight: true

                IconButton {
                    id: tabButton
                    label: tabItem.modelData
                    onClicked: {
                        root.indexRequested(tabItem.index)
                    }
                }

                Rectangle {
                    width: tabButton.width
                    height: 1
                    color: root.index == tabItem.index ? "white" : "transparent"
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Rectangle {
                width: parent.width
                height: 1
                color: "transparent"
                anchors.bottom: parent.bottom
            }
        }
    }

    Rectangle {
        id: bottomDivider
        width: parent.width
        height: 1
        color: Theme.lineColor
        anchors.top: tabs.bottom
        anchors.topMargin: -1
    }
}
