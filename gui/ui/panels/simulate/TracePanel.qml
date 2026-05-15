import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ScrollView {
    id: root

    Layout.fillHeight: true
    Layout.fillWidth: true

    contentWidth: availableWidth

    ColumnLayout {
        width: root.availableWidth

        InlineDocumentation {
            key: "placeholder_small"
            target: AppData.view.left_panel
            title: "Simulation Runner"
        }


        STPropertyPanel {
            Layout.fillWidth: true

            collapsible: false
            title: "New Job"

            STComboBox {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                model: App.simulation.runners
                textRole: "name"
                valueRole: "runner"
                currentIndex: App.simulation.runners.index_of(
                                  App.simulation.runner)
                onActivated: (index) => {
                    App.simulation.runner =
                            App.simulation.runners.runner_at(index)
                }
            }

            STPropertyLabel {
                text: "# of Rays"
            }

            STSpinBox {
                Layout.fillWidth: true
                from: 0
                value: AppData.simulation.ray_count
                to: 1000000000

                onValueModified: AppData.simulation.ray_count = value
            }

            STPropertyLabel {
                text: "Max # Rays Traced"
            }

            STSpinBox {
                Layout.fillWidth: true
                from: 0
                value: AppData.simulation.max_ray_count
                to: 1000000000

                onValueModified: AppData.simulation.ray_count = value
            }

            STPropertyLabel {
                text: "# of CPU Cores"
                visible: App.simulation.runner < 2
            }

            STSpinBox {
                Layout.fillWidth: true
                from: 1
                value: AppData.simulation.cpu_cores
                to: 64

                visible: App.simulation.runner < 2
                onValueModified: AppData.simulation.cpu_cores = value
            }

            STPropertyLabel {
                text: "Seed Value"
            }

            STSpinBox {
                Layout.fillWidth: true
                from: 1
                value: AppData.simulation.seed_value
                to: 10000000

                onValueModified: AppData.simulation.seed_value = value
            }

            STPropertyLabel {
                text: "Options"
                Layout.rowSpan: 3

                Layout.alignment: Qt.AlignRight | Qt.AlignTop
            }

            STSwitch {
                text: "Sun Error"
                checked: App.simulation.sun_error
                onToggled: App.simulation.sun_error = checked
            }

            STSwitch {
                text: "Sun Shape"
                checked: App.simulation.sun_shape
                onToggled: App.simulation.sun_shape = checked
            }

            STSwitch {
                text: "Optical Errors"
                checked: App.simulation.optical_errors
                onToggled: App.simulation.optical_errors = checked
            }

            STButton {
                Layout.fillWidth: true
                Layout.columnSpan: 2

                text: "Enqueue Job"
                text_icon: "\uf0da"

                onClicked: {
                    AppData.simulation.run()
                }
            }
        }
    }
}
