import QtQuick
import SolTraceProto

Item {
    id: root

    property string label: ""
    property bool isVisible: true

    default property alias content: node.data

    width: parent ? parent.width : 0
    implicitHeight: node.implicitHeight
    height: implicitHeight

    Component.onCompleted: {
        node.hasContent = node.data.length > 0
    }

    Callout {
        id: node
        title: root.label

        buttons: [
            Component {
                IconButton {
                    label: root.isVisible ? "Hide" : "Show"
                    source: "qrc:/icons/assets/icons/placeholder.svg"
                    onClicked: root.isVisible = !root.isVisible
                }
            }
        ]
    }
}
