import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls.Material
import SolTraceProto

Item {
    id: root
    property var source: null
    property int sidebarWidth: 300
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

            ConfigureSidebarBody {
                id: sidebarBody
                Layout.column: 0
                Layout.fillHeight: true
                Layout.preferredWidth: root.sidebarWidth
                Layout.row: 0
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


                // GlassRectangle {
                //     id: floatingToolbar
                //     anchors.horizontalCenter: parent.horizontalCenter
                //     anchors.bottom: parent.bottom
                //     anchors.bottomMargin: 20
                //     width: toolbarLayout.implicitWidth + 20
                //     height: toolbarLayout.implicitHeight + 10
                //     z: 100
                //     visible: Session.core.sectionIndex == 1

                //     RowLayout {
                //         id: toolbarLayout
                //         anchors.centerIn: parent
                //         spacing: 8

                //         IconButton {
                //             source: "qrc:/icons/assets/icons/placeholder.svg"
                //             label: "Add Material"
                //             onClicked: {
                //                 // Add material action
                //             }
                //         }

                //         Rectangle {
                //             Layout.preferredWidth: 1
                //             Layout.fillHeight: true
                //             Layout.topMargin: 8
                //             Layout.bottomMargin: 8
                //             color: Theme.lineColor
                //         }

                //         Row {
                //             spacing: 5

                //             IconButton {
                //                 source: "qrc:/icons/assets/icons/placeholder.svg"
                //                 label: "Edit Selected"
                //                 onClicked: {
                //                     // Add material action
                //                 }
                //             }

                //             IconButton {
                //                 source: "qrc:/icons/assets/icons/placeholder.svg"
                //                 label: "Edit All"
                //                 onClicked: {
                //                     // Add material action
                //                 }
                //             }
                //         }

                //         Rectangle {
                //             Layout.preferredWidth: 1
                //             Layout.fillHeight: true
                //             Layout.topMargin: 8
                //             Layout.bottomMargin: 8
                //             color: Theme.lineColor
                //         }

                //         Row {
                //             spacing: 5

                //             IconButton {
                //                 source: "qrc:/icons/assets/icons/placeholder.svg"
                //                 label: "Remove Selected"
                //                 onClicked: {
                //                     // Remove selected action
                //                 }
                //             }

                //             IconButton {
                //                 source: "qrc:/icons/assets/icons/placeholder.svg"
                //                 label: "Remove All"
                //                 onClicked: {
                //                     // Remove all action
                //                 }
                //             }
                //         }

                //         Rectangle {
                //             Layout.preferredWidth: 1
                //             Layout.fillHeight: true
                //             Layout.topMargin: 8
                //             Layout.bottomMargin: 8
                //             color: Theme.lineColor
                //         }

                //         Row {
                //             spacing: 5

                //             IconButton {
                //                 source: "qrc:/icons/assets/icons/upload.svg"
                //                 label: "Import"
                //                 onClicked: {
                //                     // Import action
                //                 }
                //             }

                //             IconButton {
                //                 source: "qrc:/icons/assets/icons/download.svg"
                //                 label: "Export"
                //                 onClicked: {
                //                     // Export action
                //                 }
                //             }
                //         }
                //     }
                // }

                // This causes some issues with other panels that want to control
                // scrolling
                //ScrollView {
                StackLayout {
                    id: contentScrollView
                    anchors.fill: parent
                    //clip: true

                    //contentWidth: availableWidth
                    //contentHeight: contentItem.height

                    MaterialsPanel {
                        //anchors.fill: parent
                        //anchors.margins: 20
                        //anchors.rightMargin: 50

                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        z: 2
                    }

                    // Loader {
                    //     id: contentItem
                    //     width: contentScrollView.availableWidth
                    //     height: implicitHeight
                    //     sourceComponent: {
                    //         switch(Session.core.sectionIndex) {
                    //             case 0: return sunPanel
                    //             case 1: return materialsPanel
                    //             case 2: return stagingPanel
                    //             case 3: return tracingPanel
                    //             default: return null
                    //         }
                    //     }

                    //     MouseArea {
                    //         anchors.fill: parent
                    //         onClicked: {
                    //             root.forceActiveFocus()
                    //             console.log("Pressed")
                    //         }
                    //         z: 1
                    //     }

                    //     Component {
                    //         id: sunPanel
                    //         SunPanel {
                    //             width: contentScrollView.availableWidth
                    //             z: 2
                    //         }
                    //     }

                    //     Component {
                    //         id: materialsPanel
                    //         Item {}
                    //     }

                    //     Component {
                    //         id: stagingPanel
                    //         StagingPanel {
                    //             width: contentScrollView.availableWidth
                    //             z: 2
                    //         }
                    //     }

                    //     Component {
                    //         id: tracingPanel
                    //         TracingPanel {
                    //             width: contentScrollView.availableWidth
                    //             z: 2
                    //         }
                    //     }
                    // }
                }
            }
        }
    }
}
