import QtQuick 2.15
import SolTraceProto

Column {
    id: root
    property var model: []
    property var childrenAction: null

    width: parent.width
    spacing: 0

    Repeater {
        model: root.model

        TreeNavigatorNode {
            nodeData: modelData
            nodeIndex: index
            titleFont: "CMU Serif"
            titleSize: 16
            titleWeight: Font.Bold

            parentActions: {
                if (root.childrenAction) {
                    return [{
                        action: root.childrenAction,
                        index: index
                    }]
                }
                return []
            }

            Component.onCompleted: {
                isLoaded = true
            }
        }
    }
}
