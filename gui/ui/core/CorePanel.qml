import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

Item {
    id: root
    property var blur_source

    TopBar {
        id: top_bar
        anchors.left: parent.left
        anchors.margins: 10
        anchors.right: parent.right
        anchors.top: parent.top
        blur_source: root.blur_source
        height: 48
    }

    onWidthChanged: {
        while (width < background.requested_width && background.requested_width_index > 0) {
            background.requested_width_index -= 1
        }
    }

    ShadowedGlassRectangle {
        id: background
        blur_source: root.blur_source

        property int requested_width_index : 1

        property var available_widths : [250, 500, 750]

        property var requested_width : available_widths[requested_width_index]

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.top: top_bar.bottom

        anchors.margins: 10

        width: requested_width

        radius: 10

        Behavior on requested_width {
            NumberAnimation {
                duration: 100
            }
        }

        // ADD HANDLERS TO STOP INPUT PROPAGATION
        // Catch clicks / presses / hover movement
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            hoverEnabled: true
            preventStealing: true

            onPressed: (mouse) => mouse.accepted = true
            onReleased: (mouse) => mouse.accepted = true
            onClicked: (mouse) => mouse.accepted = true
            onDoubleClicked: (mouse) => mouse.accepted = true
            onPositionChanged: (mouse) => mouse.accepted = true
            onWheel: (wheel) => wheel.accepted = true
        }

        StackLayout {

            id: module_stack

            anchors.fill: parent
            anchors.margins: 10

            ConfigureModule {
                size_class: background.requested_width_index
            }

        }

    }

    // TODO: Fix
    STIconButton {
        id: smaller_button
        text: "\uf060"

        anchors.bottom: background.bottom
        anchors.right: background.right

        anchors.rightMargin: - width / 2

        onClicked: {
            background.requested_width_index = Math.max(
                        background.requested_width_index - 1,
                        0,
                        )
        }
    }

    STIconButton {
        id: larger_button
        text: "\uf061"

        anchors.bottom: smaller_button.top
        anchors.right: background.right

        anchors.rightMargin: - width / 2

        onClicked: {
            background.requested_width_index = Math.min(
                        background.requested_width_index + 1,
                        background.available_widths.length - 1,
                        )
        }
    }
}
