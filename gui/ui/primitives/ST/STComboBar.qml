import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Effects

import SolTrace

Rectangle {
    id: bar

    property int currentIndex: 0

    property string fontFamily
    property var fontWeight: Font.Normal

    property var model: []

    property real padding: 10
    property Item selectedItem: null

    radius: height / 2
    color: App.theme.glassColor

    height: 42

    Rectangle {
        anchors.fill: parent
        anchors.margins: 0

        radius: height / 2
        color: "transparent"
        border.color: Qt.alpha("black", .25)

        Rectangle {
            anchors.fill: parent
            anchors.margins: 0

            radius: height / 2
            color: "transparent"
            border.color: Qt.alpha("black", .1)
        }
    }

    // Horrible.
    Label {
        id: default_label
        visible: false
    }

    Rectangle {
        id: content_item

        visible: selectedItem !== null

        property real padding: 5


        z: 0

        x: (selectedItem ? layout.x + selectedItem.x : 0) - padding
        y: (selectedItem ? layout.y + selectedItem.y : 0) - padding
        width: (selectedItem ? selectedItem.width : 0) + 2 * padding
        height: (selectedItem ? selectedItem.height : 0) + 2 * padding

        radius: height / 2

        //color: Material.backgroundColor
        color: App.theme.glassColor

        border.color: Material.dividerColor

        RectangularShadow {
            anchors.fill: parent
            offset.x: 0
            offset.y: 0
            radius: parent.radius
            blur: 30
            spread: 2
            color: Material.dropShadowColor
        }

        Behavior on x {
            NumberAnimation {
                duration: 100
            }
        }

        Behavior on y {
            NumberAnimation {
                duration: 100
            }
        }

        // Behavior on width {
        //     NumberAnimation {
        //         duration: 100
        //     }
        // }

        // Behavior on height {
        //     NumberAnimation {
        //         duration: 100
        //     }
        // }
    }

    function select(index) {
        bar.currentIndex = index
    }

    function modelCount() {
        if (!bar.model) {
            return 0
        }

        if (bar.model.length !== undefined) {
            return bar.model.length
        }

        if (bar.model.count !== undefined) {
            return bar.model.count
        }

        return 0
    }

    function syncSelection() {
        var count = modelCount()

        if (count <= 0) {
            currentIndex = -1
            selectedItem = null
            return
        }

        if (currentIndex < 0 || currentIndex >= count) {
            currentIndex = Math.max(0, Math.min(currentIndex, count - 1))
        }

        selectedItem = repeater.itemAt(currentIndex)
    }

    Component.onCompleted: syncSelection()
    onModelChanged: syncSelection()
    onCurrentIndexChanged: syncSelection()

    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: bar.padding

        uniformCellSizes: true

        Repeater {
            id: repeater
            model: bar.model

            onItemAdded: function(index, item) {
                if (repeater.count === bar.modelCount()) {
                    bar.syncSelection()
                }
            }

            onItemRemoved: function(index, item) {
                if (item === bar.selectedItem) {
                    bar.selectedItem = null
                }

                bar.syncSelection()
            }

            Item {
                id: delegate
                Layout.fillWidth: true
                Layout.fillHeight: true
                z: 0

                Label {
                    id: label

                    anchors.fill: parent

                    font.family: bar.fontFamily.length ?
                                bar.fontFamily
                                   :
                                default_label.font.family



                    text: modelData

                    font.weight: bar.fontWeight

                    horizontalAlignment: Label.AlignHCenter
                    verticalAlignment: Label.AlignVCenter

                    opacity: index === bar.currentIndex ? 1.0 : .5
                }

                Rectangle {
                    visible:
                        index !== 0 &&
                        bar.currentIndex !== index &&
                        bar.currentIndex !== index - 1
                    anchors.left: label.left
                    anchors.leftMargin: -3
                    width : 1
                    color: Material.dividerColor
                    height: label.height - 4
                    anchors.verticalCenter: label.verticalCenter
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: bar.select(index)
                }
            }
        }
    }


}
