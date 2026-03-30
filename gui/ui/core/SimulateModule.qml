import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root
    property int size_class

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
            value: App.simulation.progress * 100
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
