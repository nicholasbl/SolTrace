import QtQuick
import QtQuick.Layouts

import SolTrace

RowLayout {
    id: root

    property var blur_source
    required property int available_width

    signal show_script_area()

    readonly property bool show_logos: available_width > 1250

    ShadowedGlassRectangle {
        id: left_data_glass

        Layout.fillHeight: true
        Layout.fillWidth: true

        blur_source: root.blur_source
        radius: height / 2
        glassColor: App.theme.glassColor

        RowLayout {
            anchors.fill: parent
            spacing: 0

            TopBarLeftPane {
                Layout.fillHeight: true
                Layout.maximumWidth: root.width * 0.3

                available_width: root.available_width
                show_logos: root.show_logos
            }

            TopBarDataPane {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.preferredWidth: 600
            }
        }
    }

    TopBarRightPane {
        Layout.fillHeight: true
        Layout.maximumWidth: root.width * 0.3

        blur_source: root.blur_source
        available_width: root.available_width
    }
}
