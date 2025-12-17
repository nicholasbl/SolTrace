import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls.Material
import SolTraceProto

Item {
    id: root
    property var source: null
    property int sidebarWidth: 300
    property int headerHeight: 50
    property bool showDescriptions: false
    property bool showDiagrams: false
    property bool showExamples: false
    anchors.fill: parent

    GlassRect {
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
                    text: "(1) Configure Simulation"
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
                color: "white"
                opacity: 0.2
            }

            Item {
                id: contentHeader
                Layout.row: 0
                Layout.column: 2
                Layout.fillWidth: true
                Layout.preferredHeight: root.headerHeight

                ComboButtonGroup {
                    source: root.source
                    groups: [
                        QtObject {
                            property string label: "Documentation"
                            property list<Component> buttons: [
                                Component {
                                    IconButton {
                                        label: (root.showDescriptions ? "Hide " : "Show ") + "Descriptions"
                                        source: "qrc:/icons/assets/icons/info.svg"
                                        onClicked: root.showDescriptions = !root.showDescriptions
                                    }
                                },
                                Component {
                                    IconButton {
                                        label: (root.showDiagrams ? "Hide " : "Show ") + "Diagrams"
                                        source: "qrc:/icons/assets/icons/shapes.svg"
                                        onClicked: root.showDiagrams = !root.showDiagrams
                                    }
                                },
                                Component {
                                    IconButton {
                                        label: (root.showExamples ? "Hide " : "Show ") + "Examples"
                                        source: "qrc:/icons/assets/icons/blocks.svg"
                                        onClicked: root.showExamples = !root.showExamples
                                    }
                                },
                                Component {
                                    IconButton {
                                        label: "Play Walkthrough"
                                        source: "qrc:/icons/assets/icons/list-video.svg"
                                    }
                                }
                            ]
                        },
                        QtObject {
                            property string label: "Other"
                            property list<Component> buttons: [
                                Component {
                                    IconButton {
                                        label: "Export Config"
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
                color: "white"
                opacity: 0.2
            }

            Item {
                id: sidebarBodya
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
                        anchors.topMargin: 10

                        Row {
                            anchors.left: parent.left
                            anchors.leftMargin: 20

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

                        Callout {
                            title: "Light"
                            titleFont: "CMU Serif"
                            titleSize: 18
                            titleWeight: Font.Bold

                            Callout {
                                title: "Light Source Type & Position"

                                Callout {
                                    title: "Directional Light"
                                    hasContent: false
                                }

                                Callout {
                                    title: "Point Source"
                                    hasContent: false
                                }
                            }

                            Callout {
                                title: "Emisson Profile"

                                Callout {
                                    title: "Guassian"
                                    hasContent: false
                                }

                                Callout {
                                    title: "Pillbox"
                                    hasContent: false
                                }

                                Callout {
                                    title: "Custom"
                                    hasContent: false
                                }
                            }
                        }

                        Callout {
                            title: "Materials"
                            titleFont: "CMU Serif"
                            titleSize: 18
                            titleWeight: Font.Bold
                        }

                        Callout {
                            title: "Staging"
                            titleFont: "CMU Serif"
                            titleSize: 18
                            titleWeight: Font.Bold
                        }

                        Callout {
                            title: "Tracing"
                            titleFont: "CMU Serif"
                            titleSize: 18
                            titleWeight: Font.Bold
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

            }
        }
    }
}
