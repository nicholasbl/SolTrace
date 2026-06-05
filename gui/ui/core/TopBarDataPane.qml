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

    property bool is_in_analysis_view: App.view.simulation_content_view

    glassColor: highlighted ? Qt.alpha(Material.accentColor, 0.35) :
                              data_mouse_area.containsMouse ? App.theme.shadedGlassColor :
                                                              App.theme.glassColor

    function flash_added_data() {
        flash_highlight_animation.restart()
    }

    Behavior on glassColor {
        ColorAnimation {
            duration: 200
        }
    }

    RowLayout {
        anchors.fill: parent

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

        Label {
            Layout.fillHeight: true
            Layout.preferredWidth: implicitWidth
            Layout.leftMargin: 20

            verticalAlignment: Qt.AlignVCenter

            id: duplicate_result_button
            text: root.is_in_analysis_view ? "Analyze" : "Edit Scene"
            font.pointSize: 16
        }

        Rectangle {
            Layout.preferredWidth: 1
            Layout.fillHeight: true

            color: Material.dividerColor
        }

        Label {
            Layout.fillHeight: true
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignRight

            Layout.rightMargin: 20

            text: {
                if (root.is_in_analysis_view) {
                    return AppData.simulation.current_simulation_result_name
                }

                return App.file_source.current_database ?
                            App.file_source.current_database.name : "None"
            }
            elide: Label.ElideMiddle
            font.family: "CMU Serif"
            font.bold: true
            font.pointSize: 18


        }
    }

    Rectangle {
        anchors.fill: parent

        visible: root.is_in_analysis_view

        radius: height / 2.0

        color: "transparent"

        border.width: 2
        border.color: Material.color(Material.Yellow)
    }

    MouseArea {
        id: data_mouse_area
        anchors.fill: parent

        hoverEnabled: true

        onClicked: data_pop.open()
    }


    Item {
        anchors.fill: parent

        DataPopup {
            id: data_pop

            width: parent.width
            height: Overlay.overlay.height * 0.66
        }
    }
}
