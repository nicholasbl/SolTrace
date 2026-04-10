import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root

    STComboBar {
        id: bar

        currentIndex: App.view.simulate_section
        onCurrentIndexChanged: App.view.simulate_section  = currentIndex

        Layout.fillWidth: true

        fontFamily: App.view.left_panel.size == PanelData.Small ? "Font Awesome 7 Free" : ""

        model : App.view.left_panel.size == PanelData.Small ?
                    ["\uf828", "\uf03d", "\uf188", "\uf15c"]
                  :
                    ["Execution", "Navigation", "Diagnostics", "Logs"]

    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Material.dividerColor
    }

    StackLayout {
        currentIndex: App.view.simulate_section

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: 8

        // Execution
        ColumnLayout {
            InlineDocumentation {
                key: "placeholder_small"
                target: App.view.left_panel
                title: "Simulation Runner"
            }

            STPropertyPanel {
                Layout.fillWidth: true

                collapsible: false
                title: "New Job"

                STPropertyLabel {
                    text: "# of Rays"
                }

                STSpinBox {
                    Layout.fillWidth: true
                    from: 0
                    value: App.simulation.ray_count
                    to: 1000000000

                    onValueModified: App.simulation.ray_count = value
                }

                STPropertyLabel {
                    text: "Max # Rays Traced"
                }

                STSpinBox {
                    Layout.fillWidth: true
                    from: 0
                    value: App.simulation.max_ray_count
                    to: 1000000000

                    onValueModified: App.simulation.ray_count = value
                }

                STButton {
                    Layout.fillWidth: true
                    Layout.columnSpan: 2

                    text: "Enqueue Job"
                    text_icon: "\uf0da"

                    onClicked: {
                        App.simulation.run()
                    }
                }
            }
        }

        // Navigation
        ColumnLayout {
            InlineDocumentation {
                key: "placeholder_small"
                title: "Cameras"
                target: App.view.left_panel
            }

            STComboBar {
                currentIndex: App.view.camera
                onCurrentIndexChanged: App.view.camera  = currentIndex

                Layout.fillWidth: true

                fontFamily: App.view.left_panel.size == PanelData.Small ? "Font Awesome 7 Free" : ""

                model : App.view.left_panel.size == PanelData.Small ?
                            ["\uf828", "\uf03d"]
                          :
                            ["WASD Camera", "Orbital Camera"]
            }

            InlineDocumentation {
                key: "placeholder_small"
                title: "Camera Perspectives"
                target: App.view.left_panel
            }

            STComboBar {
                currentIndex: App.view.perspective
                onCurrentIndexChanged: App.view.perspective  = currentIndex

                Layout.fillWidth: true

                fontFamily: App.view.left_panel.size == PanelData.Small ? "Font Awesome 7 Free" : ""

                model : App.view.left_panel.size == PanelData.Small ?
                            ["\uf06a", "\uf06a"]
                          :
                            ["Normal Perspective", "Orthographic Perspective"]
            }
        }

        // Diagnostics
        ColumnLayout {
            InlineDocumentation {
                key: "placeholder_small"
                title: "Bounding Boxes"
                target: App.view.left_panel
            }

            STButton {
                text: "View Bounding Boxes"
                text_icon: ""
            }

            InlineDocumentation {
                key: "placeholder_small"
                title: "Element Volumes"
                target: App.view.left_panel
            }

            STButton {
                text: "View Element Volumes"
                text_icon: ""
            }
        }

        // Logs
        ColumnLayout {
            InlineDocumentation {
                key: "placeholder_small"
                title: "Logs"
                target: App.view.left_panel
            }

            STButton {
                text: "View Logs"
                text_icon: ""
            }
        }
    }

    STPropertyPanel {
        Layout.fillWidth: true

        collapsible: false
        title: "Status"

        enabled: App.simulation.is_running

        STPropertyLabel {
            text: "Progress"
        }

        ProgressBar {
            Layout.fillWidth: true
            from: 0
            to: 100
            value: App.simulation.progress
        }

        STPropertyLabel {
            text: "Stage"
        }

        Label {
            Layout.fillWidth: true
            text: App.simulation.current_stage
        }
    }

}
