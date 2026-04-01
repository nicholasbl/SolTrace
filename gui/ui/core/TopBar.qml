import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Dialogs

import SolTrace


RowLayout {
    id: root
    property var blur_source
    signal show_script_area()

    RowLayout {
        Layout.fillHeight: true
        Layout.preferredWidth: root.width * .25
        Layout.maximumWidth: root.width * .25

        // Left Panel Button

        ShadowedGlassRectangle {
            blur_source: root.blur_source

            radius: height / 2

            implicitWidth: row.implicitWidth

            Layout.fillHeight: true

            RowLayout {
                id: row
                anchors.fill: parent

                STIconButton {
                    id: leftpanel_open
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitHeight
                    Layout.leftMargin: 20
                    Layout.rightMargin: 10

                    text: "\uf0c9"
                    label.font.pointSize: 20
                    onClicked: App.view.show_left_panel = !App.view.show_left_panel
                }

                Column {
                    spacing: 0
                    Layout.fillWidth: true
                    Layout.rightMargin: 20
                    Layout.alignment: Qt.AlignVCenter

                    RowLayout {
                        id: logoRow
                        spacing: 8

                        Image {
                            source: "qrc:/assets/images/logo.svg"
                            Layout.preferredHeight: 27
                            Layout.preferredWidth: Layout.preferredHeight
                            Layout.alignment: Qt.AlignBottom
                            mipmap: true
                            fillMode: Image.PreserveAspectFit
                        }

                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignBottom
                            font.pointSize: 18
                            font.family: "CMU Serif"
                            text: "SolTrace"
                            font.bold: true
                            font.capitalization: Font.SmallCaps
                        }
                    }

                    Rectangle {
                        color: "white"
                        width: logoRow.width
                        height: 1
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true

                    color: Material.dividerColor
                }

                STClickableLabel {
                    Layout.leftMargin: 10
                    Layout.rightMargin: 20

                    text: "File"
                    font.pointSize: 16

                    onClicked: file_menu.open()

                    Menu {
                        id: file_menu
                        MenuItem {
                            text: "New"
                            onClicked: App.file_source.source = ""
                        }
                        MenuItem {
                            text: "Open"
                            onClicked: openFileDialog.open()
                        }
                        MenuItem { text: "Save" }
                    }

                    FileDialog {
                        id: openFileDialog
                        currentFolder: StandardPaths.standardLocations(
                                           StandardPaths.DocumentsLocation
                                           )[0]
                        onAccepted: App.file_source.source = selectedFile
                    }

                    // Popup {
                    //     id: file_menu
                    //     ColumnLayout {
                    //         anchors.fill: parent
                    //         Label  {
                    //             text: "New"
                    //             font.pointSize: 16
                    //         }
                    //         Rectangle {
                    //             Layout.fillWidth: true
                    //             Layout.preferredHeight: 1
                    //         }

                    //         Label  {
                    //             text: "Open"
                    //             font.pointSize: 16
                    //         }
                    //         Label  {
                    //             text: "Save"
                    //             font.pointSize: 16
                    //         }
                    //     }

                    //     background: Rectangle {
                    //         border.width: 1
                    //         border.color: Material.dividerColor
                    //         radius: 10
                    //         color: Qt.alpha(Material.backgroundColor, .90)
                    //     }
                    // }
                }


            }


        }

        Item {
            Layout.fillWidth: true
        }
    }

    ShadowedGlassRectangle {
        blur_source: root.blur_source

        radius: height / 2

        Layout.fillHeight: true
        Layout.fillWidth: true

        Label {
            anchors.centerIn: parent
            anchors.verticalCenterOffset: 2
            text: App.file_source.name
            font.family: "CMU Serif"
            font.bold: true
            font.pointSize: 18
        }

        /* Free up prime real estate

        RowLayout {
            anchors.fill: parent

            spacing: 10

            Item {
                Layout.fillWidth: true
            }

            STClickableLabel {
                text: "Configure"
                font.pointSize: 16

                onClicked: {
                    App.view.show_left_panel = App.view.workflow_phase != 0 || !App.view.show_left_panel
                    App.view.workflow_phase = 0
                }
            }

            Label {
                font.family: "Font Awesome 7 Free"
                text: "\uf101"
            }

            STClickableLabel {
                text: "Simulate"
                font.pointSize: 16

                onClicked: {
                    App.view.show_left_panel = App.view.workflow_phase != 1 || !App.view.show_left_panel
                    App.view.workflow_phase = 1
                }
            }

            Label {
                font.family: "Font Awesome 7 Free"
                text: "\uf101"
            }

            STClickableLabel {
                text: "Analyze"
                font.pointSize: 16

                onClicked: {
                    App.view.show_left_panel = App.view.workflow_phase != 2 || !App.view.show_left_panel
                    App.view.workflow_phase = 2
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }
        */
    }

    RowLayout {
        Layout.fillHeight: true
        Layout.preferredWidth: root.width * .25
        Layout.maximumWidth: root.width * .25

        Item {
            Layout.fillWidth: true
        }

        ShadowedGlassRectangle {
            blur_source: root.blur_source

            radius: height / 2

            Layout.fillHeight: true
            Layout.preferredWidth: settings_row.implicitWidth

            RowLayout {
                id: settings_row
                anchors.fill: parent

                STIconButton {
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitHeight
                    label.font.pointSize: 20
                    Layout.leftMargin: 20

                    text: "\uf059"
                }

                STIconButton {
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitHeight
                    label.font.pointSize: 20

                    text: "\uf013"

                }

                STIconButton {
                    id: rightpanel_open
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitHeight
                    Layout.rightMargin: 20

                    text: "\uf0c9"
                    label.font.pointSize: 20
                    onClicked: App.view.show_right_panel = !App.view.show_right_panel
                }
            }
        }
    }
}


