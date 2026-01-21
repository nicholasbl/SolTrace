import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls.Material
import SolTraceProto

Item {
    id: root
    property var source: null
    property int sidebarWidth: 325
    property var panels: ["Sun", "Materials", "Staging", "Tracing"]
    property int panelIndex: 0

    anchors.fill: parent

    GlassRectangle {
        anchors.fill: parent

        GridLayout {
            id: contentGrid
            anchors.fill: parent
            columns: 3
            rows: 1
            columnSpacing: 0
            rowSpacing: 0

            Item {
                id: sidebarBody
                Layout.row: 0
                Layout.column: 0
                Layout.preferredWidth: root.sidebarWidth
                Layout.fillHeight: true

                Column {
                    id: configurationPanel
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: Theme.lineColor
                    }

                    Spacer { height: 10 }

                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 20
                        spacing: 0

                        RowLayout {
                            width: parent.width
                            spacing: 0

                            Body {
                                text: "Configuration"
                                font.family: "CMU Serif"
                                font.bold: true
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            Row {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.topMargin: -2

                                IconButton {
                                    tooltip: "New"
                                    iconSize: 18
                                    source: "qrc:/icons/assets/icons/placeholder.svg"
                                    onClicked: {
                                        // Add action
                                    }
                                }

                                IconButton {
                                    tooltip: "Save"
                                    iconSize: 18
                                    source: "qrc:/icons/assets/icons/save.svg"
                                    onClicked: {
                                        // Add action
                                    }
                                }

                                IconButton {
                                    tooltip: "Import"
                                    iconSize: 18
                                    source: "qrc:/icons/assets/icons/download.svg"
                                    onClicked: {
                                        // Add action
                                    }
                                }

                                IconButton {
                                    tooltip: "Export"
                                    iconSize: 18
                                    source: "qrc:/icons/assets/icons/upload.svg"
                                    onClicked: {
                                        // Add action
                                    }
                                }
                            }
                        }

                        GlassComboBox {
                            model: ["Untitled (Unsaved)", "Config1 (Last Run)"]
                            width: parent.width
                            height: 30
                            labelFontSize: Theme.textSizeSmall
                            borderColor: Theme.lineColor
                        }
                    }

                    Spacer { height: 10 }

                }

                ScrollView {
                    width: parent.width
                    anchors.top: parent.top
                    anchors.bottom: configurationPanel.top
                    clip: true

                    Column {
                        anchors.fill: parent
                        anchors.margins: 20
                        width: parent.width
                        spacing: 10

                        Row {
                            IconButton {
                                label: "Collapse All"
                            }

                            IconButton {
                                label: "Expand All"
                            }
                        }

                        TreeNavigator {
                            model: {
                                switch(Session.core.sectionIndex) {
                                    case 0: // Sun
                                        return [
                                            {
                                                title: "Sun Type & Direction",
                                                childrenAction: (index) => Session.core.sun.sunTypeIndex = index,
                                                children: [
                                                    {
                                                        title: "Directional Sun",
                                                        childrenAction: (index) => Session.core.sun.calculatorIndex = index,
                                                        children: [
                                                            { title: "Legacy Calculator" },
                                                            { title: "Duffie and Beckman" },
                                                            { title: "SOLPOS" },
                                                            { title: "SPA" }
                                                        ]
                                                    },
                                                    { title: "Point Source Sun" }
                                                ]
                                            },
                                            {
                                                title: "Sun Shape",
                                                childrenAction: (index) => Session.core.sun.sunShapeIndex = index,
                                                children: [
                                                    { title: "Gaussian" },
                                                    { title: "Pillbox" },
                                                    { title: "Buie CSR" },
                                                    { title: "Custom" }
                                                ]
                                            }
                                        ]
                                    case 1: // Materials
                                        return [
                                            { title: "Invalid Materials" },
                                            { title: "Group1" },
                                            { title: "Group2" },
                                            { title: "All Materials" }
                                        ]
                                    case 2: // Staging
                                        return [
                                            { title: "Geometry" },
                                            { title: "Positioning" }
                                        ]
                                    case 3: // Tracing
                                        return [
                                            { title: "Ray Tracer" },
                                            { title: "Engine Parameters" },
                                            { title: "Execution Parameters" },
                                            { title: "Optimizations & Errors" }
                                        ]
                                    default:
                                        return []
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: verticalDivider
                Layout.row: 0
                Layout.column: 1
                Layout.preferredWidth: 1
                Layout.fillHeight: true
                color: Theme.lineColor
            }

            Item {
                id: contentBody
                Layout.row: 0
                Layout.column: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: 10


                GlassRectangle {
                    id: floatingToolbar
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 20
                    width: toolbarLayout.implicitWidth + 20
                    height: toolbarLayout.implicitHeight + 10
                    z: 100
                    visible: Session.core.sectionIndex == 1

                    RowLayout {
                        id: toolbarLayout
                        anchors.centerIn: parent
                        spacing: 8

                        IconButton {
                            source: "qrc:/icons/assets/icons/placeholder.svg"
                            label: "Add Material"
                            onClicked: {
                                // Add material action
                            }
                        }

                        Rectangle {
                            Layout.preferredWidth: 1
                            Layout.fillHeight: true
                            Layout.topMargin: 8
                            Layout.bottomMargin: 8
                            color: Theme.lineColor
                        }

                        Row {
                            spacing: 5

                            IconButton {
                                source: "qrc:/icons/assets/icons/placeholder.svg"
                                label: "Edit Selected"
                                onClicked: {
                                    // Add material action
                                }
                            }

                            IconButton {
                                source: "qrc:/icons/assets/icons/placeholder.svg"
                                label: "Edit All"
                                onClicked: {
                                    // Add material action
                                }
                            }
                        }

                        Rectangle {
                            Layout.preferredWidth: 1
                            Layout.fillHeight: true
                            Layout.topMargin: 8
                            Layout.bottomMargin: 8
                            color: Theme.lineColor
                        }

                        Row {
                            spacing: 5

                            IconButton {
                                source: "qrc:/icons/assets/icons/placeholder.svg"
                                label: "Remove Selected"
                                onClicked: {
                                    // Remove selected action
                                }
                            }

                            IconButton {
                                source: "qrc:/icons/assets/icons/placeholder.svg"
                                label: "Remove All"
                                onClicked: {
                                    // Remove all action
                                }
                            }
                        }

                        Rectangle {
                            Layout.preferredWidth: 1
                            Layout.fillHeight: true
                            Layout.topMargin: 8
                            Layout.bottomMargin: 8
                            color: Theme.lineColor
                        }

                        Row {
                            spacing: 5

                            IconButton {
                                source: "qrc:/icons/assets/icons/upload.svg"
                                label: "Import"
                                onClicked: {
                                    // Import action
                                }
                            }

                            IconButton {
                                source: "qrc:/icons/assets/icons/download.svg"
                                label: "Export"
                                onClicked: {
                                    // Export action
                                }
                            }
                        }
                    }
                }

                ScrollView {
                    id: contentScrollView
                    anchors.fill: parent
                    clip: true

                    contentWidth: availableWidth
                    contentHeight: contentItem.height

                    Loader {
                        id: contentItem
                        width: contentScrollView.availableWidth
                        height: implicitHeight
                        sourceComponent: {
                            switch(Session.core.sectionIndex) {
                                case 0: return sunPanel
                                case 1: return materialsPanel
                                case 2: return stagingPanel
                                case 3: return tracingPanel
                                default: return null
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                root.forceActiveFocus()
                                console.log("Pressed")
                            }
                            z: 1
                        }

                        Component {
                            id: sunPanel
                            SunPanel {
                                width: contentScrollView.availableWidth
                                z: 2
                            }
                        }

                        Component {
                            id: materialsPanel
                            MaterialsPanel {
                                width: contentScrollView.availableWidth
                                z: 2
                            }
                        }

                        Component {
                            id: stagingPanel
                            StagingPanel {
                                width: contentScrollView.availableWidth
                                z: 2
                            }
                        }

                        Component {
                            id: tracingPanel
                            TracingPanel {
                                width: contentScrollView.availableWidth
                                z: 2
                            }
                        }
                    }
                }
            }
        }
    }
}
