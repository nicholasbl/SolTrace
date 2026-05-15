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

        currentIndex: AppData.view.simulate_section
        onCurrentIndexChanged: AppData.view.simulate_section  = currentIndex

        Layout.fillWidth: true

        fontFamily: AppData.view.left_panel.size == PanelData.Small ? "Font Awesome 7 Free" : ""

        model : AppData.view.left_panel.size == PanelData.Small ?
                    ["\uf828", "\uf188"]
                  :
                    ["Execution", "Diagnostics"]

    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Material.dividerColor
    }

    StackLayout {
        currentIndex: AppData.view.simulate_section

        Layout.fillWidth: true
        Layout.fillHeight: true

        TracePanel {

        }

        // Logs
        ColumnLayout {
            InlineDocumentation {
                key: "placeholder_small"
                title: "Logs"
                target: AppData.view.left_panel
            }

            STButton {
                text: "View Logs"
                text_icon: ""
            }

            InlineDocumentation {
                key: "placeholder_small"
                title: "Bounding Boxes"
                target: AppData.view.left_panel
            }

            STButton {
                text: "View Bounding Boxes"
                text_icon: ""
            }

            InlineDocumentation {
                key: "placeholder_small"
                title: "Element Volumes"
                target: AppData.view.left_panel
            }

            STButton {
                text: "View Element Volumes"
                text_icon: ""
            }
        }
    }

    STPropertyPanel {
        Layout.fillWidth: true

        collapsible: false
        title: "Status"

        enabled: AppData.simulation.is_running

        STPropertyLabel {
            text: "Progress"
        }

        ProgressBar {
            Layout.fillWidth: true
            from: 0
            to: 100
            value: AppData.simulation.progress
        }

        STPropertyLabel {
            text: "Stage"
        }

        Label {
            Layout.fillWidth: true
            text: AppData.simulation.current_stage
        }
    }

}
