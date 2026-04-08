import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

Item {
    id: root

    property alias model: internal.model
    property int currentIndex: -1
    property bool editing: false

    // Width threshold for switching between stacked and side-by-side
    property int wideThreshold: 500

    // List side preferred width in wide mode
    property int listWidth: 250

    // The delegate used for each row in the list
    property Component listDelegate: null

    // Header component shown above the list (e.g. a search field)
    property Component listHeader: null

    // Footer component shown below the list (e.g. an add button)
    property Component listFooter: null

    // The component shown when an item is selected
    property Component detailView: null

    // Placeholder shown in wide mode when nothing is selected
    property Component placeholder: null

    // Read only data
    readonly property bool wideMode: width >= wideThreshold
    readonly property bool hasSelection: currentIndex >= 0

    // Signal
    signal itemClicked(int index, var modelData)

    function goBack() {
        editing = false
    }

    function clearSelection() {
        currentIndex = -1
        editing = false
    }

    QtObject {
        id: internal
        property var model: null
    }


    // Narrow stack layout
    Item {
        anchors.fill: parent
        visible: !root.wideMode

        StackView {
            id: narrowStack
            initialItem: narrowListComponent
            anchors.fill: parent
        }
    }

    Component {
        id: narrowListComponent

        ColumnLayout {
            spacing: 0

            Loader {
                Layout.fillWidth: true
                Layout.margins: 8

                sourceComponent: root.listHeader
                active: root.listHeader !== null
                visible: active
            }

            ListView {
                Layout.fillHeight: true
                Layout.fillWidth: true
                clip: true
                model: internal.model
                ScrollIndicator.vertical: ScrollIndicator { }

                delegate: Loader {
                    required property int index
                    required property var model
                    width: ListView.view.width

                    sourceComponent: root.listDelegate

                    property int itemIndex: index
                    property var itemModel: model
                    property bool isCurrent: root.currentIndex === index

                    Connections {
                        target: item
                        function onClicked() {
                            root.currentIndex = index
                            root.editing = true
                            root.itemClicked(index, model)
                        }
                    }
                }
            }

            Loader {
                Layout.fillWidth: true
                Layout.margins: 8
                sourceComponent: root.listFooter
                active: root.listFooter !== null
                visible: active
            }
        }
    }

    Component {
        id: narrowDetailComponent

        Loader {
            sourceComponent: root.detailView
            active: root.detailView !== null
        }
    }

   // Wide layout
    Item {
        anchors.fill: parent
        visible: root.wideMode

        RowLayout {
            anchors.fill: parent
            spacing: 0
            Layout.margins: 8


            ColumnLayout {
                Layout.preferredWidth: root.listWidth
                Layout.maximumWidth: root.listWidth
                Layout.fillHeight: true
                Layout.margins: 8
                spacing: 0

                Loader {
                    Layout.fillWidth: true
                    sourceComponent: root.listHeader
                    active: root.listHeader !== null
                    visible: active
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: internal.model
                    ScrollIndicator.vertical: ScrollIndicator { }

                    delegate: Loader {
                        required property int index
                        required property var model
                        width: ListView.view.width

                        sourceComponent: root.listDelegate

                        property int itemIndex: index
                        property var itemModel: model
                        property bool isCurrent: root.currentIndex === index

                        Connections {
                            target: item
                            function onClicked() {
                                root.currentIndex = index
                                root.editing = true
                                root.itemClicked(index, model)
                            }
                        }
                    }
                }

                Loader {
                    Layout.fillWidth: true
                    sourceComponent: root.listFooter
                    active: root.listFooter !== null
                    visible: active
                }
            }

            Rectangle {
                Layout.preferredWidth: 1
                Layout.fillHeight: true
                color: App.theme.dividerColor
            }

            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 8
                sourceComponent: root.hasSelection ? root.detailView : root.placeholder
                active: true
            }
        }
    }
}
