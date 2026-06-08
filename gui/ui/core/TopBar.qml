import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material

import SolTrace

RowLayout {
    id: root

    property var blur_source
    required property int available_width

    signal show_script_area()

    readonly property bool show_logos: available_width > 900

    ShadowedGlassRectangle {
        id: left_data_glass

        Layout.fillHeight: true
        //Layout.fillWidth: true
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

            Rectangle {
                Layout.fillHeight: true
                width: 1
                color: Material.dividerColor
            }

            TopBarDataPane {
                id: top_bar_data_pane

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.preferredWidth: implicitWidth
            }
        }
    }

    Item {
        Layout.fillWidth: true
    }

    TopBarRightPane {
        Layout.fillHeight: true
        Layout.maximumWidth: root.width * 0.3

        blur_source: root.blur_source
        available_width: root.available_width
    }
}
