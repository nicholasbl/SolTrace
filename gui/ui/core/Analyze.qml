import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import SolTraceProto

Item {
    id: root
    property var source: null
    property int sidebarWidth: 300
    property int headerHeight: 50

    anchors.fill: parent

    GlassRectangle {
        anchors.fill: parent
        source: root.source

        GridLayout {
            id: contentGrid
            anchors.fill: parent
            columns: 3
            rows: 3
            columnSpacing: 0
            rowSpacing: 0

            Item {
                id: sidebarHeader
                Layout.row: 0
                Layout.column: 0
                Layout.preferredWidth: root.sidebarWidth
                Layout.preferredHeight: root.headerHeight

                Text {
                    text: "(3) Analyze Simulation"
                    color: "white"
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    font.family: "CMU Serif"
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: 5
                }
            }

            Rectangle {
                id: verticalDivider
                Layout.row: 0
                Layout.column: 1
                Layout.preferredWidth: 1
                Layout.rowSpan: 3
                Layout.fillHeight: true
                color: Theme.lineColor
            }

            Item {
                id: contentHeader
                Layout.row: 0
                Layout.column: 2
                Layout.fillWidth: true
                Layout.preferredHeight: root.headerHeight

                ComboButtonGrouBody {
                    source: root.source
                    groups: [DocumentationButtonGrouBody {},
                        QtObject {
                            property string label: "Other"
                            property list<Component> buttons: [
                                Component {
                                    IconButton {
                                        label: "Export Results"
                                        source: "qrc:/icons/assets/icons/placeholder.svg"
                                    }
                                }
                            ]
                        }
                    ]
                }
            }

            Rectangle {
                id: horizontalDivider
                Layout.row: 1
                Layout.column: 0
                Layout.columnSpan: 3
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: Theme.lineColor
            }

            Item {
                id: sidebarBody
                Layout.row: 2
                Layout.column: 0
                Layout.preferredWidth: root.sidebarWidth
                Layout.fillHeight: true

                ScrollView {
                    width: parent.width
                    height: parent.height - 60
                    clip: true

                    Column {
                        anchors.fill: parent
                        anchors.margins: 20
                        anchors.topMargin: 10

                        Row {
                            anchors.left: parent.left

                            IconButton {
                                label: "Collapse All"
                            }

                            IconButton {
                                label: "Expand All"
                            }
                        }

                        Item {
                            width: 10
                            height: 10
                        }


                        TreeNavigator {
                            childrenAction: (index) => Session.core.sectionIndex = index
                            model: [
                                { title: "Flux Maps" },
                                { title: "Data" }
                            ]
                        }

                    }
                }

            }

            Item {
                id: contentBody
                Layout.row: 2
                Layout.column: 2
                Layout.fillWidth: true
                Layout.fillHeight: true

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
