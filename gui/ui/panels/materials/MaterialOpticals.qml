import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

PropertyPanel {
    id: root
    property bool has_selection : false
    property var material_editor : App.materials.group_edit
    property var side_editor

    // =========================================================================

    PropertyLabel {
        text: "Ideal presets"
    }

    RowLayout {
        Layout.fillWidth: true

        Button {
            Layout.fillWidth: true
            text: "Absorber"
            onClicked: root.side_editor.set_ideal_absorption()
        }

        Button {
            Layout.fillWidth: true
            text: "Reflector"
            onClicked: root.side_editor.set_ideal_reflection()
        }

        Button {
            Layout.fillWidth: true
            text: "Transmitter"
            onClicked: root.side_editor.set_ideal_transmission()
        }
    }

    // =========================================================================

    PropertyLabel {
        text: "Interaction"
    }

    GlassComboBox {
        Layout.fillWidth: true
        model: App.materials.group_edit.interaction_type_model

        Component.onCompleted: {
            console.log(model)
        }

        ////borderColor: Theme.lineColor
        enabled: count > 0
        //currentText: root.editor.interaction_type
        onActivated: {
            if (root.editor && currentText.length > 0) {
                root.editor.interaction_type = currentText
            }
        }
    }

    // =========================================================================

    PropertyLabel {
        text: "Distribution"
    }

    GlassComboBox {
        id: front_distribution
        Layout.fillWidth: true
        model: App.materials.group_edit.distribution_type_model
        //borderColor: Theme.lineColor
        enabled: count > 0
        onActivated: {
            if (root.editor && currentText.length > 0) {
                root.editor.error_distribution_type = currentText
            }
        }
    }

    // =========================================================================

    PropertyLabel {
        text: "Reflectance (ρ)"
    }

    GlassSpinBox {
        Layout.fillWidth: true
        from: 0
        to: decimalToInt(1)
        decimals: 4
        stepSize: decimalToInt(0.01)
    }

    // =========================================================================

    PropertyLabel {
        text: "Transmittance (τ)"
    }

    GlassSpinBox {
        Layout.fillWidth: true
        from: 0
        to: decimalToInt(1)
        decimals: 4
        stepSize: decimalToInt(0.01)
    }

    // =========================================================================

    PropertyLabel {
        text: "<em>n</em> Front"
    }

    GlassSpinBox {
        Layout.fillWidth: true
        from: decimalToInt(1)
        to: decimalToInt(5)
        decimals: 4
        stepSize: decimalToInt(0.01)
    }

    // =========================================================================

    PropertyLabel {
        text: "<em>n</em> Back"
    }

    GlassSpinBox {
        Layout.fillWidth: true
        from: decimalToInt(1)
        to: decimalToInt(5)
        decimals: 4
        stepSize: decimalToInt(0.01)
    }

    // =========================================================================

    PropertyLabel {
        text: "Slope Error (σ<sub>slope</sub>)"
    }

    GlassSpinBox {
        Layout.fillWidth: true
        from: decimalToInt(1)
        to: decimalToInt(1000)
        decimals: 3
        stepSize: decimalToInt(0.01)
        suffix: "mrad"
    }

    // =========================================================================

    PropertyLabel {
        text: "Specularity Error (σ<sub>spec</sub>)"
    }

    GlassSpinBox {
        Layout.fillWidth: true
        from: decimalToInt(1)
        to: decimalToInt(1000)
        decimals: 3
        stepSize: decimalToInt(0.01)
        suffix: "mrad"
    }

    // =========================================================================

    PropertyLabel {
        text: "Error Type"
    }

    GlassComboBox {
        Layout.fillWidth: true
        //model: App.materials
        ////borderColor: Theme.lineColor
        enabled: count > 0
        //currentText: root.editor.interaction_type
        onActivated: {
            if (root.editor && currentText.length > 0) {
                root.editor.interaction_type = currentText
            }
        }
    }

    // =========================================================================

    // RowLayout {
    //     Layout.columnSpan: 2
    //     Layout.fillWidth: true
    //     spacing: 12

    //     Button {
    //         Layout.fillWidth: true
    //         text: "Angular Reflectance"
    //         enabled: false
    //     }

    //     Button {
    //         Layout.fillWidth: true
    //         text: "Angular Transmittance"
    //         enabled: false
    //     }
    // }
}
