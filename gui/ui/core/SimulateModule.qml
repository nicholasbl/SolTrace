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
        onCurrentIndexChanged: AppData.view.simulate_section = currentIndex
        Layout.fillWidth: true
        collapseLabels: AppData.view.left_panel.size === PanelData.Small
        model: ["Execution", "Diagnostics"]
        iconModel: ["\uf04b", "\uf188"]
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

        STPropertyLabel {
            text: "Progress"
        }

        ProgressBar {
            Layout.fillWidth: true
            from: 0
            to: 100
            value: AppData.simulation.progress

            enabled: AppData.simulation.is_running
        }

        STPropertyLabel {
            text: "Stage"
        }

        Label {
            Layout.fillWidth: true
            text: AppData.simulation.is_running ?
                      AppData.simulation.current_stage : "Idle"
        }

        STButton {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            text: "View Results"
            text_icon: "\uf1da"
            onClicked: {
                App.view.left_panel.visible = true
                App.view.simulation_content_view = true
                App.view.analyze_section = 0
                App.view.workflow_phase = 2
            }
        }
    }

}
