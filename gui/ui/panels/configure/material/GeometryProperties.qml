import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import SolTrace

ColumnLayout {
    id: root

    property var group_edit: App.materials.geometry_edit

    spacing: 8

    STPropertyPanel {
        Layout.fillWidth: true

        title: "Surface Properties"
        collapsible: true

        STPropertyLabel {
            text: "Surface Type"

            Layout.row: 0
            Layout.column: 0
        }

        STComboBox {
            id: surfaceTypeCombo
            Layout.fillWidth: true
            Layout.row: 0
            Layout.column: 1
            model: root.group_edit.surface_parameter_model.surface_type_model
            textRole: "display"
            valueRole: "display"

            currentValue: root.group_edit.surface_parameter_model.surface_kind

            onActivated: {
                root.group_edit.surface_parameter_model.surface_kind = currentText
            }
        }

        Repeater {
            model: root.group_edit.surface_parameter_model
            delegate: STPropertyLabel {
                required property int index
                required property string name

                text: name

                Layout.row: index + 1
                Layout.column: 0
            }
        }

        Repeater {
            model: root.group_edit.surface_parameter_model
            delegate: STDoubleSpinBox {
                required property int index
                required property var model
                required property real content
                required property real min
                required property real max

                Layout.fillWidth: true
                Layout.row: index + 1
                Layout.column: 1

                value: content
                from: min
                to: max
                stepSize: 0.01
                onValueModified: model.content = value
            }
        }
    }

    STPropertyPanel {
        Layout.fillWidth: true

        title: "Aperture Properties"
        collapsible: true

        STPropertyLabel {
            text: "Aperture Type"

            Layout.row: 0
            Layout.column: 0
        }

        STComboBox {
            id: apertureTypeCombo
            Layout.fillWidth: true
            Layout.row: 0
            Layout.column: 1
            model: root.group_edit.aperture_parameter_model.aperture_type_model
            textRole: "display"
            valueRole: "display"

            currentValue: root.group_edit.aperture_parameter_model.aperture_kind

            onActivated: {
                root.group_edit.aperture_parameter_model.aperture_kind = currentText
            }
        }

        Repeater {
            model: root.group_edit.aperture_parameter_model
            delegate: STPropertyLabel {
                required property int index
                required property string name

                text: name

                Layout.row: index + 1
                Layout.column: 0
            }
        }

        Repeater {
            model: root.group_edit.aperture_parameter_model
            delegate: STDoubleSpinBox {
                required property int index
                required property var model
                required property real content
                required property real min
                required property real max

                Layout.fillWidth: true
                Layout.row: index + 1
                Layout.column: 1

                value: content
                from: min
                to: max
                stepSize: 0.01
                onValueModified: model.content = value
            }
        }
    }
}
