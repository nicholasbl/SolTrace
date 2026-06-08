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

    required property int available_width
    required property bool show_logos

    spacing: 0

    Binding { target: soltrace_logo; property: "visible"; value: root.show_logos }
    Binding { target: prerel_logo; property: "visible"; value: root.show_logos }

            STIconButton {
                id: leftpanel_open
                Layout.preferredWidth: implicitWidth
                Layout.preferredHeight: implicitHeight
                Layout.leftMargin: 20
                Layout.rightMargin: 10

                text: "\uf0c9"
                toolTip: (App.view.left_panel.visible ? "Close": "Open") + " Left Panel"

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

            Item {
                id: soltrace_logo
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                implicitWidth: logo_content.implicitWidth
                implicitHeight: logo_content.implicitHeight

                Rectangle {
                    anchors.fill: logo_content
                    anchors.margins: -5
                    radius: 5

                    color: logo_mouse_area.containsMouse ? Material.rippleColor :
                                                           "transparent"

                    Behavior on color {
                        ColorAnimation {
                            duration: 100
                        }
                    }
                }

                Column {
                    id: logo_content
                    spacing: -2

                    RowLayout {
                        id: logo_row
                        spacing: 4

                        Item { Layout.preferredWidth: 2 }

                        Image {
                            id: logo_icon
                            source: "qrc:/assets/images/logo.svg"
                            Layout.preferredHeight: 27
                            Layout.preferredWidth: Layout.preferredHeight
                            Layout.alignment: Qt.AlignBottom
                            mipmap: true
                            fillMode: Image.PreserveAspectFit

                            layer.enabled: true
                            layer.effect: MultiEffect {
                                colorization: 1.0
                                colorizationColor: App.theme.fontColor
                            }
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

                        Item { Layout.preferredWidth: 2 }
                    }

                    Rectangle {
                        color: App.theme.fontColor
                        width: logo_row.width
                        height: 1
                    }
                }

                MouseArea {
                    id: logo_mouse_area
                    anchors.fill: logo_content
                    hoverEnabled: true
                    onClicked: version_pop.open()
                }

                STPopup {
                    id: version_pop

                    contentWidth: 280

                    ColumnLayout {
                        spacing: 6

                        Label {
                            Layout.fillWidth: true

                            text: "SolTrace"
                            font.bold: true
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        }

                        Label {
                            Layout.fillWidth: true

                            text: AppData.current_build_info
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        }
                    }
                }
            }

            STClickableLabel {
                id: prerel_logo

                visible: AppData.is_prerelease

                Layout.alignment: Qt.AlignVCenter
                Layout.topMargin: 10
                Layout.rightMargin: 10

                text: "BETA"
                font.pointSize: 10
                font.family: "CMU Serif"
                font.bold: true
                font.italic: true

                color: Material.color(Material.Yellow)

                onClicked: beta_pop.open()

                STPopup {
                    id: beta_pop

                    contentWidth: 280

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 6

                        Label {
                            Layout.fillWidth: true

                            text: "Pre-release build"
                            font.bold: true
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        }

                        Label {
                            Layout.fillWidth: true

                            text: "This version of SolTrace is intended for testing and evaluation. Features, file formats, and simulation behavior may change before the final release. Verify important results with a stable release before using them for production work."
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        }
                    }
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

                borderWidth: 0

                text: "File"
                font.pointSize: 16

                onClicked: file_menu.open()

                STMenu {
                    id: file_menu
                    MenuItem {
                        text: "New"
                        onClicked: App.file_source.load_new()

                        enabled: !AppData.file_source.is_loading
                    }
                    MenuItem {
                        text: "Open"
                        onClicked: openFileDialog.open()

                        enabled: !AppData.file_source.is_loading
                    }

                    STMenu {
                        id: recents_menu
                        title: qsTr("Recent Files")
                        enabled: recent_instantiator.count > 0

                        Instantiator {
                            id: recent_instantiator
                            model: file_settings.recent_files

                            delegate: MenuItem {
                                // Show file name only. Settings may restore
                                // entries as strings or QUrl-like values, so
                                // normalize before doing string operations.
                                text: file_settings.file_name(modelData)
                                onTriggered: {
                                    file_menu.dismiss()
                                    var file_url = file_settings.file_url_text(modelData)
                                    file_settings.add_files(file_url)
                                    App.file_source.load_url(Qt.url(file_url))
                                }
                            }

                            onObjectAdded: (index, object) => recents_menu.insertItem(index, object)
                            onObjectRemoved: (index, object) => recents_menu.removeItem(object)
                        }

                        MenuSeparator {
                            visible: recent_instantiator.count > 0
                        }

                        MenuItem {
                            text: qsTr("Clear Menu")
                            onTriggered: file_settings.clear_recent_files()
                        }
                    }

                    MenuItem {
                        text: "Save"

                        enabled: !AppData.file_source.is_loading
                    }
                }

                QtObject {
                    id: file_settings

                    property var recent_files: []
                    property url last_selected_file: ""
                    property url last_selected_folder: StandardPaths.standardLocations(
                                                           StandardPaths.DocumentsLocation)[0]

                    function file_url_text(file_path) {
                        return String(file_path)
                    }

                    function file_name(file_path) {
                        var file_url = file_url_text(file_path)
                        return decodeURIComponent(file_url.split('/').pop())
                    }

                    function recent_files_array() {
                        var files = []

                        if (!recent_files) {
                            return files
                        }

                        for (var i = 0; i < recent_files.length; ++i) {
                            files.push(file_url_text(recent_files[i]))
                        }

                        return files
                    }

                    function set_recent_files(files) {
                        recent_files = files
                    }

                    function clear_recent_files() {
                        set_recent_files([])
                    }

                    function add_files(file_path) {
                        var normalized_file_path = file_url_text(file_path)
                        var files = recent_files_array()
                        var index = files.indexOf(normalized_file_path)

                        if (index !== -1) {
                            files.splice(index, 1)
                        }

                        files.unshift(normalized_file_path)

                        if (files.length > 5) {
                            files.pop()
                        }

                        // Assign a fresh array so the Instantiator model
                        // receives a change notification.
                        set_recent_files(files)
                    }
                }

                Settings {
                    id: file_settings_storage

                    category: "file_history"

                    property alias recent_files: file_settings.recent_files
                    property alias last_selected_file: file_settings.last_selected_file
                    property alias last_selected_folder: file_settings.last_selected_folder
                }

                FileDialog {
                    id: openFileDialog

                    currentFolder: file_settings.last_selected_folder
                    selectedFile: file_settings.last_selected_file

                    onAccepted: {
                        var str_file = String(selectedFile)

                        file_settings.last_selected_file = selectedFile
                        file_settings.last_selected_folder = str_file.substring(0, str_file.lastIndexOf("/"))
                        file_settings.add_files(selectedFile.toString())
                        App.file_source.load_url(selectedFile)
                    }
                }
            }
        }
