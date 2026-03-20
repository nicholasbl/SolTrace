import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SolTrace

STPropertyPanel {
    id: root
    property bool has_selection : false
    property var material_editor : App.materials.group_edit
    property var side_editor

    function syncComboToValue(combo, value) {
        if (!combo || !combo.count || !value || value.length === 0) {
            return
        }

        for (let i = 0; i < combo.count; i++) {
            if (combo.textAt(i) === value) {
                combo.currentIndex = i
                return
            }
        }
    }

    onSide_editorChanged: {
        syncComboToValue(interactionCombo,
                         root.side_editor ? root.side_editor.interaction_type : "")
        syncComboToValue(distributionCombo,
                         root.side_editor ? root.side_editor.error_distribution_type : "")
    }

    // =========================================================================

    STPropertyLabel {
        text: "Ideal presets"
    }

    RowLayout {
        Layout.fillWidth: true

        Button {
            Layout.fillWidth: true
            text: "Absorber"
            onClicked: {
                if (root.side_editor) {
                    root.side_editor.set_ideal_absorption()
                }
            }
        }

        Button {
            Layout.fillWidth: true
            text: "Reflector"
            onClicked: {
                if (root.side_editor) {
                    root.side_editor.set_ideal_reflection()
                }
            }
        }

        Button {
            Layout.fillWidth: true
            text: "Transmitter"
            onClicked: {
                if (root.side_editor) {
                    root.side_editor.set_ideal_transmission()
                }
            }
        }
    }

    // =========================================================================

    STPropertyLabel {
        text: "Interaction"
    }

    STComboBox {
        id: interactionCombo
        Layout.fillWidth: true
        model: App.materials.group_edit.interaction_type_model
        textRole: "display"

        onActivated: {
            if (root.side_editor && currentText.length > 0) {
                root.side_editor.interaction_type = currentText
            }
        }

        Connections {
            target: root.side_editor
            function onInteraction_type_changed() {
                root.syncComboToValue(interactionCombo, root.side_editor.interaction_type)
            }
        }
    }



    // =========================================================================

    STPropertyLabel {
        text: "Distribution"
    }

    STComboBox {
        id: distributionCombo
        Layout.fillWidth: true
        model: App.materials.group_edit.distribution_type_model
        textRole: "display"
        enabled: count > 0
        onCountChanged: {
            root.syncComboToValue(distributionCombo,
                                  root.side_editor ? root.side_editor.error_distribution_type : "")
        }
        onModelChanged: {
            root.syncComboToValue(distributionCombo,
                                  root.side_editor ? root.side_editor.error_distribution_type : "")
        }
        onActivated: {
            if (root.side_editor && currentText.length > 0) {
                root.side_editor.error_distribution_type = currentText
            }
        }


        Connections {
            target: root.side_editor
            function onError_distribution_type_changed() {
                root.syncComboToValue(distributionCombo,
                                      root.side_editor ? root.side_editor.error_distribution_type : "")
            }
        }
    }


    // =========================================================================

    STPropertyLabel {
        text: "Reflectance (ρ)"
    }

    STSpinBox {
        Layout.fillWidth: true
        from: 0
        to: decimalToInt(1)
        decimals: 4
        stepSize: decimalToInt(0.01)
        value: decimalToInt(root.side_editor ? root.side_editor.reflectivity : 0)
        onRealValueModified: {
            if (root.side_editor) {
                root.side_editor.reflectivity = realValue
            }
        }
    }

    // =========================================================================

    STPropertyLabel {
        text: "Transmittance (τ)"
    }

    STSpinBox {
        Layout.fillWidth: true
        from: 0
        to: decimalToInt(1)
        decimals: 4
        stepSize: decimalToInt(0.01)
        value: decimalToInt(root.side_editor ? root.side_editor.transmitivity : 0)
        onRealValueModified: {
            if (root.side_editor) {
                root.side_editor.transmitivity = realValue
            }
        }
    }

    // =========================================================================

    STPropertyLabel {
        text: "<em>n</em> Front"
        textFormat: Label.RichText
    }

    STSpinBox {
        Layout.fillWidth: true
        from: decimalToInt(1)
        to: decimalToInt(5)
        decimals: 4
        stepSize: decimalToInt(0.01)
        value: decimalToInt(root.side_editor ? root.side_editor.refraction_index_front : 1)
        onRealValueModified: {
            if (root.side_editor) {
                root.side_editor.refraction_index_front = realValue
            }
        }
    }

    // =========================================================================

    STPropertyLabel {
        text: "<em>n</em> Back"
        textFormat: Label.RichText
    }

    STSpinBox {
        Layout.fillWidth: true
        from: decimalToInt(1)
        to: decimalToInt(5)
        decimals: 4
        stepSize: decimalToInt(0.01)
        value: decimalToInt(root.side_editor ? root.side_editor.refraction_index_back : 1)
        onRealValueModified: {
            if (root.side_editor) {
                root.side_editor.refraction_index_back = realValue
            }
        }
    }

    // =========================================================================

    STPropertyLabel {
        text: "Slope Error (σ<sub>slope</sub>)"
        textFormat: Label.RichText
    }

    STSpinBox {
        Layout.fillWidth: true
        from: decimalToInt(0)
        to: decimalToInt(1000)
        decimals: 3
        stepSize: decimalToInt(0.01)
        suffix: "mrad"
        value: decimalToInt(root.side_editor ? root.side_editor.slope_error : 0)
        onRealValueModified: {
            if (root.side_editor) {
                root.side_editor.slope_error = realValue
            }
        }
    }

    // =========================================================================

    STPropertyLabel {
        text: "Specularity Error (σ<sub>spec</sub>)"
        textFormat: Label.RichText
    }

    STSpinBox {
        Layout.fillWidth: true
        from: decimalToInt(0)
        to: decimalToInt(1000)
        decimals: 3
        stepSize: decimalToInt(0.01)
        suffix: "mrad"
        value: decimalToInt(root.side_editor ? root.side_editor.specularity_error : 0)
        onRealValueModified: {
            if (root.side_editor) {
                root.side_editor.specularity_error = realValue
            }
        }
    }

    // =========================================================================

    STPropertyLabel {
        text: "Error Type"
        visible: false
    }

    STComboBox {
        Layout.fillWidth: true
        model: App.materials.group_edit.distribution_type_model
        textRole: "display"
        enabled: count > 0
        visible: false
        onActivated: {
            if (root.side_editor && currentText.length > 0) {
                root.side_editor.error_distribution_type = currentText
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
