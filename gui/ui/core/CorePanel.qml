import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

Item {
    id: root
    property var blur_source

    Binding {
        target: module_stack
        property: "currentIndex"
        value: App.view.workflow_phase
    }

    enum SizeClass {
            Small = 0,
            Normal = 1,
            Wide = 2
    }

    TopBar {
        id: top_bar
        anchors.left: parent.left
        anchors.margins: 10
        anchors.right: parent.right
        anchors.top: parent.top
        blur_source: root.blur_source
        height: 48

        onShow_script_area: {
            right_stack.enabled = !right_stack.enabled
        }
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

        RowLayout {
            id: module_info_row
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 10
            height: 34

            uniformCellSizes: true

            Item {
                Layout.fillWidth: true
                visible: background.requested_width_index > 0
                Layout.fillHeight: true
            }

            Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: ["Configure", "Simulate", "Analyze"][App.view.workflow_phase]
                font.pointSize: 18
                font.bold: true

                elide: Label.ElideRight

                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignVCenter

            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                RowLayout {
                    anchors.fill: parent

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    STIconButton {
                        Layout.preferredWidth: implicitHeight
                        Layout.fillHeight: true
                        id: smaller_button
                        text: "\uf422"

                        visible: background.requested_width_index !== CorePanel.Small

                        onClicked: {
                            background.requested_width_index = Math.max(
                                        background.requested_width_index - 1,
                                        0,
                                        )
                        }
                    }

                    STIconButton {
                        Layout.preferredWidth: implicitHeight
                        Layout.fillHeight: true
                        id: larger_button
                        text: "\uf065"

                        visible: background.requested_width_index !== CorePanel.Wide

                        onClicked: {
                            background.requested_width_index = Math.min(
                                        background.requested_width_index + 1,
                                        background.available_widths.length - 1,
                                        )
                        }
                    }

                }
            }


        }

        StackLayout {
            id: module_stack

            onCurrentIndexChanged: App.view.workflow_phase

            anchors.top: module_info_row.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10

            ConfigureModule {
                size_class: background.requested_width_index
            }

            SimulateModule {
                size_class: background.requested_width_index
            }

        }
    }

    RightStack {
        id: right_stack

        opacity: enabled

        enabled: false

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }

        blur_source: root.blur_source

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: top_bar.bottom

        anchors.margins: 10

        width: Math.min(250, root.width - background.width - 50);
    }
}
