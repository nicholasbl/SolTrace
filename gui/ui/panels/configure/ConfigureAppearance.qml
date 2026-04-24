import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root

    STPropertyPanel {
        Layout.fillWidth: true

        title: "Sun Visualization"
        collapsible: false

        ColumnLayout {
            CheckBoxField {
                Layout.preferredWidth: 200

                text: "Visible"
                value: App.view.sim.sun_viz

                onClicked: {
                    App.view.sim.sun_viz = !App.view.sim.sun_viz
                }
            }

            STSpinBoxField {
                Layout.preferredWidth: 200

                label: "Scale Factor"
                value: App.view.sim.sun_viz_scale
                onValueModified: {
                    App.view.sim.sun_viz_scale = value
                }
                from: 0
                to: 100
            }
        }
    }

    STPropertyPanel {
        Layout.fillWidth: true

        title: "Geometry"
        collapsible: false

        ColorPickerField {
            id: colorPicker
            color: App.view.sim.geometry_color
            label: "Receiver Color"
            onUpdated: {
                App.view.sim.geometry_color = colorPicker.color
            }
        }
    }

}
