import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs

import SolTrace

Item {
    id: root

    implicitWidth: mode_row.implicitWidth + mode_row.anchors.leftMargin
                   + mode_row.anchors.rightMargin
    implicitHeight: mode_row.implicitHeight

    property int last_db_count: AppData.file_source.rowCount()
    property bool highlighted: false
    readonly property string active_name: App.file_source.current_database ?
                                             App.file_source.current_database.name : "None"
    readonly property string active_result_name: App.simulation.current_simulation_result_name

    function flash_added_data() {
        flash_highlight_animation.restart()
    }

    RowLayout {
        id: mode_row

        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20

        spacing: 8

        Connections {
            target: AppData.file_source

            function onRowsInserted(parent, first, last) {
                root.last_db_count = AppData.file_source.rowCount()
                if (last >= first) {
                    root.flash_added_data()
                }
            }

            function onRowsRemoved(parent, first, last) {
                root.last_db_count = AppData.file_source.rowCount()
                flash_highlight_animation.stop()
                root.highlighted = false
            }
        }

        SequentialAnimation {
            id: flash_highlight_animation

            loops: 3

            ScriptAction {
                script: root.highlighted = true
            }

            PauseAnimation {
                duration: 400
            }

            ScriptAction {
                script: root.highlighted = false
            }

            PauseAnimation {
                duration: 400
            }
        }

        STClickableLabel {
            id: file_label

            Layout.fillHeight: true
            Layout.leftMargin: 4
            Layout.rightMargin: 4

            borderWidth: 0

            text: "File"
            font.pointSize: 16
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter

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

        Label {
            id: file_separator

            Layout.leftMargin: 4
            Layout.rightMargin: 4
            Layout.alignment: Qt.AlignVCenter

            font.family: "Font Awesome 7 Free"
            text: "\uf101"

            property bool highlight: current_scene_label.is_active

            opacity: highlight ? 1.0 : .50
        }

        STClickableLabel {
            id: current_scene_label

            property bool is_active: App.view.workflow_phase === 0

            Layout.fillHeight: true

            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter

            text: is_active ? "Configure: " + root.active_name : "Configure"
            elide: Label.ElideMiddle

            //font.bold: is_active
            font.pointSize: 16
            opacity: is_active ? 1.0 : .50

            onClicked: {
                if (App.view.workflow_phase === 0) {
                    data_pop.open()
                } else {
                    App.view.workflow_phase = 0
                }
            }

            Behavior on opacity {
                NumberAnimation {
                    duration: 150
                }
            }
        }

        STClickableLabel {
            id: swap_data_indicator
            text: "\uf0d7"

            font.family: "Font Awesome 7 Free"

            visible: App.view.workflow_phase === 0

            onClicked: data_pop.open()
        }

        Label {
            id: configure_separator

            Layout.leftMargin: 4
            Layout.rightMargin: 4
            Layout.alignment: Qt.AlignVCenter

            font.family: "Font Awesome 7 Free"
            text: "\uf101"

            property bool highlight: current_scene_label.is_active || simulate_label.is_active

            opacity: highlight ? 1.0 : .50

        }

        STClickableLabel {
            id: simulate_label

            property bool is_active: App.view.workflow_phase === 1

            Layout.fillHeight: true

            text: is_active ? "Simulate: " + root.active_name : "Simulate"
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter

            //font.bold: is_active
            font.pointSize: 16
            opacity: is_active ? 1.0 : .50

            onClicked: App.view.workflow_phase = 1

            Behavior on opacity {
                NumberAnimation {
                    duration: 150
                }
            }
        }

        STClickableLabel {
            id: swap_simulation_data_indicator
            text: "\uf0d7"

            font.family: "Font Awesome 7 Free"

            visible: App.view.workflow_phase === 1

            onClicked: data_pop.open()
        }

        Label {
            id: analyze_separator

            Layout.leftMargin: 4
            Layout.rightMargin: 4
            Layout.alignment: Qt.AlignVCenter

            font.family: "Font Awesome 7 Free"
            text: "\uf101"

            property bool highlight: analyze_label.is_active || simulate_label.is_active

            opacity: highlight ? 1.0 : .50
        }

        STClickableLabel {
            id: analyze_label

            property bool is_active: App.view.workflow_phase === 2

            Layout.fillHeight: true

            font.pointSize: 16

            text: is_active ? "Analyze: " + root.active_result_name : "Analyze"
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter

            //font.bold: is_active
            opacity: is_active ? 1.0 : .50

            onClicked: {
                if (App.view.workflow_phase === 2) {
                    results_pop.open()
                } else {
                    App.view.workflow_phase = 2
                }
            }

            Behavior on opacity {
                NumberAnimation {
                    duration: 150
                }
            }
        }

        STClickableLabel {
            id: swap_results_indicator
            text: "\uf0d7"

            font.family: "Font Awesome 7 Free"

            visible: App.view.workflow_phase === 2

            onClicked: results_pop.open()
        }

    }

    Item {
        anchors.fill: parent

        ResultsPopup {
            id: results_pop

            width: root.width
            height: Overlay.overlay.height * 0.66
        }


        DataPopup {
            id: data_pop

            width: root.width
            height: Overlay.overlay.height * 0.66
        }
    }
}
