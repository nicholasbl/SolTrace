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

            STButton {
                text: "Enqueue Job"
                text_icon: "\uf0da"
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
}
