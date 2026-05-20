import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import SolTrace

ColumnLayout {
    id: root

    property var group_edit: App.materials.geometry_edit
    property bool singleCol: App.view.left_panel.size === PanelData.Small
    property var labelAlignment: (singleCol ? Qt.AlignLeft : Qt.AlignRight) | Qt.AlignVCenter

    spacing: 8

    component ParamLabelColumn: STPropertyLabel {
        required property int index
        required property string name
        text: name
        Layout.alignment: root.labelAlignment
    }

    // Have to manually position inside of grid
    component ParamLabelGrid : STPropertyLabel {
        required property int index
        required property string name
        text: name
        Layout.row: index + 1
        Layout.column: 0
    }

    component ParamSpinBoxColumn : STDoubleSpinBox {
        required property int index
        required property var model
        required property real content
        required property real min
        required property real max
        Layout.fillWidth: true
        value: content
        from: min
        to: max
        stepSize: 0.01
        onValueModified: model.content = value
    }

    component ParamSpinBoxGrid : STDoubleSpinBox {
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

    component ParamGroup : ColumnLayout {
        required property int index
        required property string name
        required property var model
        required property real content
        required property real min
        required property real max

        ParamLabelColumn{
            index: parent.index
            name: parent.name
        }

        ParamSpinBoxColumn {
            index: parent.index
            model: parent.model
            content: parent.content
            min: parent.min
            max: parent.max
        }
    }

    // Surface Properties (2 column)
    STPropertyPanel {
        Layout.fillWidth: true
        visible: !root.singleCol
        title: "Surface Properties"
        collapsible: true
        columns: 2

        STPropertyLabel {
            text: "Surface Type"
            Layout.row: 0
            Layout.column: 0
            Layout.alignment: root.labelAlignment
        }

        STComboBox {
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
            Layout.row: 0
            Layout.column: 1
            model: root.group_edit.surface_parameter_model.surface_type_model
            textRole: "display"
            valueRole: "display"
            currentValue: root.group_edit.surface_parameter_model.surface_kind
            onActivated: root.group_edit.surface_parameter_model.surface_kind = currentText
        }

        Repeater {
            model: root.group_edit.surface_parameter_model
            delegate: ParamLabelGrid {}
        }

        Repeater {
            model: root.group_edit.surface_parameter_model
            delegate: ParamSpinBoxGrid {}
        }
    }

    // Surface Properties (1 column)
    STPropertyPanel {
        Layout.fillWidth: true
        visible: root.singleCol
        title: "Surface Properties"
        collapsible: true
        columns: 1

        STPropertyLabel {
            text: "Surface Type"
            Layout.alignment: root.labelAlignment
        }

        STComboBox {
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
            model: root.group_edit.surface_parameter_model.surface_type_model
            textRole: "display"
            valueRole: "display"
            currentValue: root.group_edit.surface_parameter_model.surface_kind
            onActivated: root.group_edit.surface_parameter_model.surface_kind = currentText
        }

        Repeater {
            model: root.group_edit.surface_parameter_model
            delegate: ParamGroup {}
        }
    }

    // Aperture Properties (2 column)
    STPropertyPanel {
        Layout.fillWidth: true
        visible: !root.singleCol
        title: "Aperture Properties"
        collapsible: true
        columns: 2

        STPropertyLabel {
            text: "Aperture Type"
            Layout.row: 0
            Layout.column: 0
            Layout.alignment: root.labelAlignment
        }

        STComboBox {
            Layout.fillWidth: true
            Layout.row: 0
            Layout.column: 1
            model: root.group_edit.aperture_parameter_model.aperture_type_model
            textRole: "display"
            valueRole: "display"
            currentValue: root.group_edit.aperture_parameter_model.aperture_kind
            onActivated: root.group_edit.aperture_parameter_model.aperture_kind = currentText
        }

        Repeater {
            model: root.group_edit.aperture_parameter_model
            delegate: ParamLabelGrid {}
        }

        Repeater {
            model: root.group_edit.aperture_parameter_model
            delegate: ParamSpinBoxGrid {}
        }
    }

    // Aperture Properties (1 column)
    STPropertyPanel {
        Layout.fillWidth: true
        visible: root.singleCol
        title: "Aperture Properties"
        collapsible: true
        columns: 1

        STPropertyLabel {
            text: "Aperture Type"
            Layout.alignment: root.labelAlignment
        }

        STComboBox {
            Layout.fillWidth: true
            model: root.group_edit.aperture_parameter_model.aperture_type_model
            textRole: "display"
            valueRole: "display"
            currentValue: root.group_edit.aperture_parameter_model.aperture_kind
            onActivated: root.group_edit.aperture_parameter_model.aperture_kind = currentText
        }

        Repeater {
            model: root.group_edit.aperture_parameter_model
            delegate: ParamGroup {}
        }
    }
}
