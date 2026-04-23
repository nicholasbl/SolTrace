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
    required property int available_width

    signal show_script_area()

    property bool show_logos: available_width > 1250 // hardcoded value

    Binding { target: soltrace_logo; property: "visible"; value: show_logos }
    Binding { target: beta_logo; property: "visible"; value: show_logos }

    RowLayout {
        id: left_pane

        Layout.fillHeight: true
        Layout.preferredWidth: root.width * .3
        Layout.maximumWidth: root.width * .3

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
                    onClicked: {
                        if (App.view.settings_panel.visible) {
                            App.view.settings_panel.visible = false
                            return
                        }

                        App.view.left_panel.visible = !App.view.left_panel.visible && !App.view.settings_panel.visible
                        App.view.fit_panels(root.available_width)
                    }
                }

                Column {
                    id: soltrace_logo
                    spacing: -2
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    RowLayout {
                        id: logo_row
                        spacing: 4

                        Item {Layout.preferredWidth: 2}

                        Image {
                            id: logo_icon
                            source: "qrc:/assets/images/logo.svg"
                            Layout.preferredHeight: 27
                            Layout.preferredWidth: Layout.preferredHeight
                            Layout.alignment: Qt.AlignBottom
                            mipmap: true
                            fillMode: Image.PreserveAspectFit
                        }

                        Label {
                            id: logo_text
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignBottom
                            font.pointSize: 18
                            font.family: "CMU Serif"
                            text: "SolTrace"
                            font.bold: true
                            font.capitalization: Font.SmallCaps
                        }

                        Item {Layout.preferredWidth: 2}
                    }

                    Rectangle {
                        color: "white"
                        width: logo_row.width
                        height: 1
                    }
                }

                Label {
                    id: beta_logo

                    Layout.alignment: Qt.AlignVCenter
                    Layout.topMargin: 10
                    Layout.rightMargin: 20

                    text: "BETA"
                    font.pointSize: 10
                    font.family: "CMU Serif"
                    font.bold: true
                    font.italic: true
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
                            onClicked: App.file_source.load_new()
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
                        onAccepted: App.file_source.load_url(selectedFile)
                    }
                }


            }


        }

        Item {
            Layout.fillWidth: true
        }
    }

    ShadowedGlassRectangle {
        id: middle_pane

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
    }

    RowLayout {
        id: right_pane

        Layout.fillHeight: true
        Layout.preferredWidth: root.width * .3
        Layout.maximumWidth: root.width * .3

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

                /* Move inline doc toggling to panel buttons
                STIconButton {
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitHeight
                    label.font.pointSize: 20
                    Layout.leftMargin: 20

                    text: "\uf059"
                } */

                STIconButton {
                    id: settings_button
                    property bool show_left_panel: false
                    property bool show_right_panel: false

                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitHeight
                    Layout.leftMargin: 20

                    label.font.pointSize: 20

                    text: "\uf013"

                    Connections {
                        target: App.view.settings_panel
                        function onVisible_changed() {
                            if (App.view.settings_panel.visible) {
                                settings_button.show_left_panel = App.view.left_panel.visible
                                App.view.left_panel.visible = false

                                settings_button.show_right_panel = App.view.right_panel.visible
                                App.view.right_panel.visible = false
                            } else {
                                App.view.left_panel.visible = settings_button.show_left_panel
                                App.view.right_panel.visible = settings_button.show_right_panel
                                App.view.fit_panels(root.available_width, false, true)
                            }
                        }
                    }

                    onClicked: {
                        App.view.settings_panel.visible = !App.view.settings_panel.visible
                    }

                }

                STIconButton {
                    id: rightpanel_open
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitHeight
                    Layout.rightMargin: 20

                    text: "\uf0c9"
                    label.font.pointSize: 20
                    onClicked: {
                        if (App.view.settings_panel.visible) {
                            App.view.settings_panel.visible = false
                            return
                        }

                        App.view.right_panel.visible = !App.view.right_panel.visible && !App.view.settings_panel.visible
                        App.view.fit_panels(root.available_width, true)
                    }
                }
            }
        }
    }
}


