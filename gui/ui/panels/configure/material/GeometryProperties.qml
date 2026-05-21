import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import SolTrace

ColumnLayout {
    id: root

    property var group_edit: App.materials.geometry_edit
    property bool singleColumn: App.view.left_panel.size === PanelData.Small
    property var labelAlignment: (singleColumn ? Qt.AlignLeft : Qt.AlignRight) | Qt.AlignVCenter

    spacing: 8

    component PropertySection : STPropertyPanel {
        id: section
        required property var parameterModel
        required property string typeLabel
        required property var typeModel
        required property string currentKind
        signal kindChanged(string kind)

        Layout.fillWidth: true
        collapsible: true
        columns: 2

        STPropertyLabel {
            text: section.typeLabel
            Layout.row: 0
            Layout.column: 0
            Layout.columnSpan: root.singleColumn ? 2 : 1
            Layout.alignment: root.labelAlignment
        }

        STComboBox {
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
            Layout.row: root.singleColumn ? 1 : 0
            Layout.column: root.singleColumn ? 0 : 1
            Layout.columnSpan: root.singleColumn ? 2 : 1
            model: section.typeModel
            textRole: "display"
            valueRole: "display"
            currentValue: section.currentKind
            onActivated: section.kindChanged(currentText)
        }

        Repeater {
            model: section.parameterModel
            delegate: STPropertyLabel {
                required property int index
                required property string name
                text: name
                Layout.row: root.singleColumn ? (index * 2 + 2) : (index + 1)
                Layout.column: 0
                Layout.columnSpan: root.singleColumn ? 2 : 1
                Layout.alignment: root.labelAlignment
            }
        }

        Repeater {
            model: section.parameterModel
            delegate: STDoubleSpinBox {
                required property int index
                required property var model
                required property real content
                required property real min
                required property real max
                Layout.fillWidth: true
                Layout.row: root.singleColumn ? (index * 2 + 3) : (index + 1)
                Layout.column: root.singleColumn ? 0 : 1
                Layout.columnSpan: root.singleColumn ? 2 : 1
                value: content
                from: min
                to: max
                stepSize: 0.01
                onValueModified: model.content = value
            }
        }
    }

    PropertySection {
        title: "Surface Properties"
        typeLabel: "Surface Type"
        parameterModel: root.group_edit.surface_parameter_model
        typeModel: root.group_edit.surface_parameter_model.surface_type_model
        currentKind: root.group_edit.surface_parameter_model.surface_kind
        onKindChanged: (kind) => root.group_edit.surface_parameter_model.surface_kind = kind
    }

    PropertySection {
        title: "Aperture Properties"
        typeLabel: "Aperture Type"
        parameterModel: root.group_edit.aperture_parameter_model
        typeModel: root.group_edit.aperture_parameter_model.aperture_type_model
        currentKind: root.group_edit.aperture_parameter_model.aperture_kind
        onKindChanged: (kind) => root.group_edit.aperture_parameter_model.aperture_kind = kind
    }
}
