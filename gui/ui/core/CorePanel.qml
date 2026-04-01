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
        while (width < background.requested_width && App.view.left_panel_size > 0) {
            App.view.left_panel_size -= 1
        }
    }

    ShadowedGlassRectangle {
        id: background
        blur_source: root.blur_source

        property var available_widths : [250, 500, 750]

        property var requested_width : available_widths[App.view.left_panel_size]

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.top: top_bar.bottom

        anchors.margins: 10

        width: requested_width

        radius: 10

        visible: App.view.show_left_panel

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

            Item {
                Layout.fillWidth: true
                visible: App.view.left_panel_size !== ViewModule.Small
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Item {
                    Layout.preferredWidth: 5 // Spacing for Small mode
                    visible: App.view.left_panel_size == ViewModule.Small
                }

                Item {
                    Layout.fillWidth: true  // Center when not in Small mode
                    visible: App.view.left_panel_size !== ViewModule.Small
                }

                Repeater {
                    model: ["Configure", "Simulate", "Analyze"]

                    RowLayout {
                        required property int index
                        required property string modelData
                        property var icons: ["\uf0ad", "\uf185", "\ue473"]

                        spacing: 10

                        // FA icon (always visible)
                        STClickableLabel {
                            text: parent.icons[parent.index]
                            font.family: "Font Awesome 7 Free"
                            opacity: App.view.workflow_phase === parent.index ? 1 : 0.5
                            font.pointSize: App.view.left_panel_size === ViewModule.Small ? 16 : 12

                            onClicked: App.view.workflow_phase = parent.index
                        }

                        // Section name (hidden in Small mode)
                        STClickableLabel {
                            font.pointSize: 16
                            font.bold: true

                            text: parent.modelData

                            font.family: "CMU Serif"
                            font.underline: App.view.workflow_phase === parent.index && App.view.left_panel_size !== ViewModule.Small

                            opacity: App.view.workflow_phase === parent.index ? 1 : 0.5
                            visible: App.view.left_panel_size !== ViewModule.Small

                            onClicked: App.view.workflow_phase = parent.index
                        }

                        // creates the ">>" icons
                        Label {
                            font.family: "Font Awesome 7 Free"
                            text: "\uf101"
                            visible: parent.index < 2
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    visible: App.view.left_panel_size !== ViewModule.Small
                }
            }

            RowLayout {
                Layout.minimumWidth: implicitWidth
                spacing: 5

                STIconButton {
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitWidth
                    id: smaller_button
                    text: "\uf422"

                    visible: App.view.left_panel_size !== ViewModule.Small

                    onClicked: {
                        App.view.left_panel_size = Math.max(
                                    App.view.left_panel_size - 1,
                                    0,
                                    )
                    }
                }

                STIconButton {
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitWidth
                    id: larger_button
                    text: "\uf065"

                    visible: App.view.left_panel_size !== ViewModule.Wide

                    onClicked: {
                        App.view.left_panel_size = Math.min(
                                    App.view.left_panel_size + 1,
                                    background.available_widths.length - 1,
                                    )
                    }
                }

                STIconButton {
                    Layout.preferredWidth: implicitWidth
                    Layout.preferredHeight: implicitWidth
                    id: close_button
                    text: "\uf00d"
                    onClicked: App.view.show_left_panel = false
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
            }

            SimulateModule {
            }

            AnalysisModule {
            }

        }
    }

    RightStack {
        id: right_stack

        opacity: enabled

        enabled: App.view.show_right_panel

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
