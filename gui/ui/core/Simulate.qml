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

    GlassRectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 300
        height: 300

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 7

            Item  {
                Layout.fillWidth: true
                Layout.preferredHeight: geometryRow.height

                Text {
                    text: "Parameters"
                    color: "white"
                    font.pixelSize: 17
                    font.weight: Font.Bold
                    font.family: "CMU Serif"
                    anchors.verticalCenter: parent.verticalCenter
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
            }
        }
    }

    ColumnLayout {
        width: 350
        height: parent.height
        spacing: 10

        GlassRectangle {
            id: commandPane
            Layout.fillWidth: true
            Layout.preferredWidth: root.sidebarWidth
            Layout.fillHeight: true
            Layout.preferredHeight: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 7

                Text {
                    text: "Simulation Runner"
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

                        onClicked: Backend.job_backend.start();
                    }
                    IconButton {
                        label: "Stop"
                        source: "qrc:/icons/assets/icons/stop.svg"

                        onClicked: Backend.job_backend.stop();
                    }
                    IconButton {
                        label: "Logs"
                        source: "qrc:/icons/assets/icons/files.svg"
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
                        source: "qrc:/icons/assets/icons/clock.svg"
                    }
                }
            }
        }

        GlassRectangle {
            id: browserPane
            Layout.fillWidth: true
            Layout.preferredWidth: root.sidebarWidth
            Layout.fillHeight: true
            Layout.preferredHeight: 2

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 7

                RowLayout {
                    id: geometryRow
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight

                    Text {
                        text: "Geometry"
                        color: "white"
                        font.pixelSize: 17
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
                            source: "qrc:/icons/assets/icons/import.svg"
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
                    id: geometryBrowserScrollView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    Column {
                        id: browserList
                        width: geometryBrowserScrollView.width
                        anchors.topMargin: 10

                        Row {

                            IconButton {
                                tooltip: "Select All"
                                source: "qrc:/icons/assets/icons/square-check.svg"
                            }
                            IconButton {
                                tooltip: "Deselect All"
                                source: "qrc:/icons/assets/icons/square-minus.svg"
                            }

                            IconButton {
                                tooltip: "Expand All"
                                source: "qrc:/icons/assets/icons/expand.svg"
                            }

                            IconButton {
                                tooltip: "Collapse All"
                                source: "qrc:/icons/assets/icons/collapse.svg"
                            }
                        }


                        // Working on BrowserTreeNode component
                        // Temporary callout placeholders
                        Callout {
                            title: "Group1"
                            titleActions: [Component { IconButton { source: "qrc:/icons/assets/icons/eye.svg"}}]

                            Callout {
                                title: "Element1"
                                titleActions: [Component { IconButton { source: "qrc:/icons/assets/icons/eye.svg"}}]
                            }

                            Callout {
                                title: "Element2"
                                titleActions: [Component { IconButton { source: "qrc:/icons/assets/icons/eye.svg"}}]
                            }
                        }
                    }
                }
            }
        }
    }
}
