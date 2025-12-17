import QtQuick

Row {
    id: root

    property list<QtObject> groups
    property var buttonGroups: []
    property int currentIndex: 0
    property var source

    spacing: 10
    anchors.fill: parent
    anchors.margins: 20

    GlassCombo {
        model: {
            let labels = []
            if (root.groups && root.groups.length > 0) {
                for (let i = 0; i < root.groups.length; i++) {
                    labels.push(root.groups[i].label)
                }
            }
            return labels
        }
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 2
        source: root.source
        labelFontSize: 14
        onCurrentIndexChanged: root.currentIndex = currentIndex
    }

    Repeater {
        model: root.groups[root.currentIndex]?.buttons || []
        delegate: Loader {
            required property var modelData
            anchors.verticalCenter: parent ? parent.verticalCenter : undefined
            sourceComponent: modelData
        }
    }
}
