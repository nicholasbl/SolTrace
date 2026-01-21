import QtQuick 2.15
import SolTraceProto

Callout {
    id: root

    property bool isLoaded: false
    property var nodeData: ({})
    property int nodeIndex: 0
    property var parentActions: []

    title: nodeData.title || "Untitled"
    titleFont: nodeData.titleFont || ""
    titleSize: nodeData.titleSize || 14
    titleWeight: nodeData.titleWeight || Font.Normal

    hasContent: false

    onIsLoadedChanged: {
        // Force boolean output with !!
        hasContent = !!(nodeData && nodeData.children && nodeData.children.length > 0)
        if (hasContent) {
            set_chevron_icon()
        } else {
            set_dot_icon()
        }
    }

    onClicked: {
        if (!parentActions || parentActions.length === 0) {
            return
        }

        for (var i = 0; i < parentActions.length; i++) {
            var actionObj = parentActions[i]

            if (actionObj && actionObj.action && typeof actionObj.action === 'function') {
                actionObj.action(actionObj.index)
            } else {
            }
        }
    }

    Repeater {
        model: (nodeData && nodeData.children) ? nodeData.children : []
        delegate: Loader {
            width: parent ? parent.width : 0
            source: "TreeNavigatorNode.qml"
            asynchronous: false  // Keep synchronous

            property var childData: modelData
            property int childIndex: index

            onLoaded: {
                if (!item) return
                item.nodeData = childData
                item.nodeIndex = childIndex

                // Use Qt.binding to create a live binding
                item.parentActions = Qt.binding(function() {
                    var actions = (root.parentActions && Array.isArray(root.parentActions)) ? root.parentActions.slice() : []
                    if (root.nodeData && root.nodeData.childrenAction && typeof root.nodeData.childrenAction === 'function') {
                        actions.push({
                            action: root.nodeData.childrenAction,
                            index: childIndex
                        })
                    }
                    return actions
                })

                item.isLoaded = true
            }
        }
    }
}
