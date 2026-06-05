import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

ShadowedGlassRectangle {
    id: root

    radius: height / 2

    property int last_db_count: AppData.file_source.rowCount()
    property bool highlighted: false

    function mode_width(is_active) {
        return mode_row.mode_control_width * (is_active ? 2 : 1)
    }

    glassColor: App.theme.glassColor

    function flash_added_data() {
        flash_highlight_animation.restart()
    }

    Behavior on glassColor {
        ColorAnimation {
            duration: 200
        }
    }

    RowLayout {
        id: mode_row

        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20

        spacing: 8

        readonly property real separator_width: configure_separator.implicitWidth
                                                + analyze_separator.implicitWidth
        readonly property real mode_control_width: Math.max(
                                                       60,
                                                       (width - separator_width
                                                        - spacing * 4) / 4)

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

        Item {
            id: configure_mode
            property bool is_active: App.view.workflow_phase === 0
            property real preferred_width: root.mode_width(is_active)

            Layout.fillHeight: true
            Layout.preferredWidth: preferred_width
            Layout.minimumWidth: 90

            Behavior on preferred_width {
                NumberAnimation {
                    duration: 250
                    easing.type: Easing.OutElastic
                }
            }

            RowLayout {
                anchors.fill: parent
                spacing: 4

                STClickableLabel {
                    id: current_scene_label

                    property string active_name: App.file_source.current_database ?
                                                     App.file_source.current_database.name : "None"
                    property real animated_point_size: configure_mode.is_active ? 16 : 15

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0

                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter

                    text: configure_mode.is_active ? "Editing: " + active_name : "Configure"
                    elide: Label.ElideMiddle

                    font.bold: configure_mode.is_active
                    font.pointSize: animated_point_size
                    opacity: configure_mode.is_active ? 1.0 : .50

                    onClicked: App.view.workflow_phase = 0

                    Behavior on animated_point_size {
                        NumberAnimation {
                            duration: 150
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 150
                        }
                    }
                }

                STIconButton {
                    visible: configure_mode.is_active

                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitHeight
                    Layout.rightMargin: 10

                    text: "\uf107"
                    iconSize: 18
                    toolTip: "Scene List"

                    onClicked: data_pop.open()
                }
            }
        }

        Label {
            id: configure_separator

            Layout.leftMargin: 4
            Layout.rightMargin: 4
            Layout.alignment: Qt.AlignVCenter

            font.family: "Font Awesome 7 Free"
            text: "\uf101"
        }

        Item {
            id: simulate_mode
            property bool is_active: App.view.workflow_phase === 1
            property real preferred_width: root.mode_width(is_active)

            Layout.fillHeight: true
            Layout.preferredWidth: preferred_width
            Layout.minimumWidth: 80

            Behavior on preferred_width {
                NumberAnimation {
                    duration: 250
                    easing.type: Easing.OutElastic
                }
            }



            STClickableLabel {
                id: simulate_label

                property real animated_point_size: simulate_mode.is_active ? 16 : 15

                anchors.fill: parent

                text: "Simulate"
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter

                font.bold: simulate_mode.is_active
                font.pointSize: animated_point_size
                opacity: simulate_mode.is_active ? 1.0 : .50

                onClicked: App.view.workflow_phase = 1

                Behavior on animated_point_size {
                    NumberAnimation {
                        duration: 150
                        easing.type: Easing.InOutQuad
                    }
                }

                Behavior on opacity {
                    NumberAnimation {
                        duration: 150
                    }
                }
            }
        }

        Label {
            id: analyze_separator

            Layout.leftMargin: 4
            Layout.rightMargin: 4
            Layout.alignment: Qt.AlignVCenter

            font.family: "Font Awesome 7 Free"
            text: "\uf101"
        }

        Item {
            id: analyze_mode
            property bool is_active: App.view.workflow_phase === 2
            property real preferred_width: root.mode_width(is_active)

            Layout.fillHeight: true
            Layout.preferredWidth: preferred_width
            Layout.minimumWidth: 80

            Behavior on preferred_width {
                NumberAnimation {
                    duration: 250
                    easing.type: Easing.OutElastic
                }
            }

            RowLayout {
                anchors.fill: parent
                spacing: 4

                STClickableLabel {
                    id: analyze_label

                    property real animated_point_size: analyze_mode.is_active ? 16 : 15

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0

                    text: "Analyze"
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter

                    font.bold: analyze_mode.is_active
                    font.pointSize: animated_point_size
                    opacity: analyze_mode.is_active ? 1.0 : .50

                    onClicked: App.view.workflow_phase = 2

                    Behavior on animated_point_size {
                        NumberAnimation {
                            duration: 150
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 150
                        }
                    }
                }

                STIconButton {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitHeight
                    Layout.rightMargin: 10

                    visible: analyze_mode.is_active

                    text: "\uf107"
                    iconSize: 18
                    toolTip: "Simulation Results"

                    onClicked: results_pop.open()
                }
            }
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
