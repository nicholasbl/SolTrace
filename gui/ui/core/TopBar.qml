import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    property var blur_source
    required property int available_width

    signal show_script_area()

    readonly property bool show_logos: available_width > 1250

    TopBarLeftPane {
        Layout.fillHeight: true
        Layout.preferredWidth: root.width * 0.3
        Layout.maximumWidth: root.width * 0.3

        blur_source: root.blur_source
        available_width: root.available_width
        show_logos: root.show_logos
    }

    TopBarDataPane {
        Layout.fillHeight: true
        Layout.fillWidth: true

        blur_source: root.blur_source
    }

    TopBarRightPane {
        Layout.fillHeight: true
        Layout.preferredWidth: root.width * 0.3
        Layout.maximumWidth: root.width * 0.3

        blur_source: root.blur_source
        available_width: root.available_width
    }
}
