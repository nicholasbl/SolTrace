import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import SolTrace

RowLayout {
    id: root

    property var blur_source
    required property int available_width

    signal show_script_area()

    readonly property bool show_logos: available_width > 900
    readonly property bool analyzing: App.view.workflow_phase === 3
    readonly property string current_context_title: analyzing ? "Analyze" : "Scene"
    readonly property string current_context_name: analyzing
                                                   ? (App.simulation.current_simulation_result_name.length ?
                                                          App.simulation.current_simulation_result_name
                                                        : "None")
                                                   : (App.file_source.current_database ?
                                                          App.file_source.current_database.name
                                                        : "None")

    ShadowedGlassRectangle {
        id: left_control_glass

        Layout.fillHeight: true
        Layout.preferredWidth: left_data_row.implicitWidth

        blur_source: root.blur_source
        radius: height / 2
        glassColor: App.theme.glassColor

        RowLayout {
            id: left_data_row

            anchors.fill: parent
            spacing: 0

            TopBarLeftPane {
                id: top_bar_left_pane

                Layout.fillHeight: true
                Layout.maximumWidth: root.width * 0.3

                available_width: root.available_width
                show_logos: root.show_logos
            }
        }
    }

    RowLayout {
        id: context_row

        Layout.leftMargin: 18
        Layout.rightMargin: 18
        spacing: 8

        Item {
            Layout.fillWidth: true
        }

        Label {
            text: root.current_context_title
            font.bold: true
            opacity: 0.65
            elide: Text.ElideRight
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.topMargin: 10
            Layout.bottomMargin: 10
            Layout.preferredWidth: 1
            color: Material.dividerColor
        }

        Label {
            Layout.maximumWidth: 320
            text: root.current_context_name
            elide: Text.ElideMiddle
        }

        Item {
            Layout.fillWidth: true
        }
    }

    TopBarRightPane {
        Layout.fillHeight: true
        Layout.maximumWidth: root.width * 0.3

        blur_source: root.blur_source
        available_width: root.available_width
    }
}
