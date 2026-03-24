import QtQuick
import QtQuick.Layouts
import SolTraceProto

Callout {
    title: "Example"
    titleFont: "CMU Serif"
    titleWeight: Font.Bold
    titleSize: 17

    titleActions: [
        Component { IconButton {
            source: "qrc:/icons/assets/icons/filter.svg"
        }},
        Component { IconButton {
            source: "qrc:/icons/assets/icons/sort-desc.svg"
        }},
        Component {
            Rectangle {
                color: "transparent"
                border.color: Theme.lineColor
                border.width: 1
                width: 250
                height: 30
                radius: 10

                RowLayout {
                    id: searchBoxContent
                    spacing: 10
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10

                    IconButton {
                        source: "qrc:/icons/assets/icons/search.svg"
                        label: ""
                        iconSize: 18
                        margins: 0
                        Layout.alignment: Qt.AlignVCenter
                    }

                    GlassTextField {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 25
                        borderColor: "transparent"
                        activeFocusBorderColor: "transparent"
                        horizontalAlignment: TextInput.AlignLeft
                        leftPadding: 5
                        rightPadding: 5
                        bottomPadding: 5
                    }
                }
            }
        }
    ]
}
