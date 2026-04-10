import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import SolTrace

STPropertyPanel {
    //title: "Geometry"
    //collapsed: false

    property var group_edit: App.materials.geometry_edit

    STPropertyLabel {
        text: "Surface Type"
    }

    STComboBox {
        id: surfaceTypeCombo
        Layout.fillWidth: true
        model: group_edit.surface_type_model
        textRole: "display"

        onActivated: {
            group_edit.surface_kind = currentText
        }
    }

    STPropertyLabel {
        text: "Aperture Type"
    }

    STComboBox {
        id: apertureTypeCombo
        Layout.fillWidth: true
        model: group_edit.aperture_type_model
        textRole: "display"

        onActivated: {
            group_edit.aperture_kind = currentText
        }
    }
}
