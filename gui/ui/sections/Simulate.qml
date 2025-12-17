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

    ColumnLayout {
        width: 350
        height: parent.height
        spacing: 10

        GlassRect {
            id: commandPane
            source: root.source
            Layout.fillWidth: true
            Layout.preferredWidth: root.sidebarWidth
            Layout.fillHeight: true
            Layout.preferredHeight: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 7

                Text {
                    text: "(2) Run Simulation"
                    color: "white"
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    font.family: "CMU Serif"
                    Layout.alignment: Qt.AlignHCenter
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "white"
                    opacity: 0.2
                }

                Row {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 5

                    IconButton {
                        label: "Run"
                        source: "qrc:/icons/assets/icons/play.svg"
                    }
                    IconButton {
                        label: "Stop"
                        source: "qrc:/icons/assets/icons/placeholder.svg"
                    }
                    IconButton {
                        label: "Logs"
                        source: "qrc:/icons/assets/icons/placeholder.svg"
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"
                    radius: 25
                    border {
                        color: Qt.rgba(255, 255, 255, 0.2)
                        width: 1
                    }

                    Text {
                        id: output

                        text: "Abbreviated debug output goes here"
                        color: "white"
                        anchors.centerIn: parent
                    }
                }

                Row {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 5

                    IconButton {
                        label: "2m 32s elapsed"
                        source: "qrc:/icons/assets/icons/placeholder.svg"
                    }
                }
            }
        }

        GlassRect {
            id: browserPane
            source: root.source
            Layout.fillWidth: true
            Layout.preferredWidth: root.sidebarWidth
            Layout.fillHeight: true
            Layout.preferredHeight: 2

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 7

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "Browser"
                        color: "white"
                        font.pixelSize: 16
                        font.weight: Font.Bold
                        font.family: "CMU Serif"
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Row {
                        spacing: 5
                        Layout.alignment: Qt.AlignRight

                        IconButton {
                            label: "Import"
                            source: "qrc:/icons/assets/icons/placeholder.svg"
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "white"
                    opacity: 0.2
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    Column {
                        id: browserList
                        width: parent.width

                        Row {

                            IconButton {
                                tooltip: "Select All"
                                source: "qrc:/icons/assets/icons/placeholder.svg"
                            }
                            IconButton {
                                tooltip: "Deselect All"
                                source: "qrc:/icons/assets/icons/placeholder.svg"
                            }

                            IconButton {
                                tooltip: "Expand All"
                                source: "qrc:/icons/assets/icons/placeholder.svg"
                            }

                            IconButton {
                                tooltip: "Collapse All"
                                source: "qrc:/icons/assets/icons/placeholder.svg"
                            }
                        }


                        // Working on BrowserTreeNode component
                        // Temporary callout placeholders
                        Callout {
                            title: "Group1"

                            Callout {
                                title: "Element1"

                            }

                            Callout {
                                title: "Element2"

                            }
                        }
                    }
                }
            }
        }
    }
}
