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

        fontFamily: App.view.left_panel_size == ViewModule.Small ? "Font Awesome 7 Free" : ""

        model : App.view.left_panel_size == ViewModule.Small ?
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
        Layout.margins: 10

        // Execution
        ColumnLayout {
            STButton {
                text: "Enqueue Job"
                text_icon: "\uf0da"
            }
        }

        // Navigation
        ColumnLayout {
            Label {
                text: "Camera"
                font.pointSize: 16
                font.family: "CMU Serif"
                font.bold: true
            }

            STComboBar {
                currentIndex: App.view.camera
                onCurrentIndexChanged: App.view.camera  = currentIndex

                Layout.fillWidth: true

                fontFamily: App.view.left_panel_size == ViewModule.Small ? "Font Awesome 7 Free" : ""

                model : App.view.left_panel_size == ViewModule.Small ?
                            ["\uf828", "\uf03d"]
                          :
                            ["WASD", "Orbital"]
            }

            Label {
                text: "Perspective"
                font.pointSize: 16
                font.family: "CMU Serif"
                font.bold: true
            }

            STComboBar {
                currentIndex: App.view.perspective
                onCurrentIndexChanged: App.view.perspective  = currentIndex

                Layout.fillWidth: true

                fontFamily: App.view.left_panel_size == ViewModule.Small ? "Font Awesome 7 Free" : ""

                model : App.view.left_panel_size == ViewModule.Small ?
                            ["\uf06a", "\uf06a"]
                          :
                            ["Normal", "Orthographic"]
            }
        }

        // Diagnostics
        ColumnLayout {
            STButton {
                text: "View Bounding Boxes"
                text_icon: ""
            }

            STButton {
                text: "View Element Volumes"
                text_icon: ""
            }
        }

        // Logs
        ColumnLayout {
            STButton {
                text: "View Logs"
                text_icon: ""
            }
        }
    }
}
