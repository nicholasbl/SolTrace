import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

PropertyPanel {
    title: "Geometry"
    collapsed: false
    enabled: right_side.hasSelection && right_side.groupEditor
    
    PropertyLabel {
        text: "Aperture Type"
    }
    
    GlassComboBox {
        id: apertureTypeCombo
        Layout.fillWidth: true
        model: right_side.groupEditor ? right_side.groupEditor.aperture_type_model : null
        textRole: "display"
        enabled: count > 0
        onCountChanged: {
            right_side.syncComboToValue(apertureTypeCombo,
                                        right_side.groupEditor ? right_side.groupEditor.aperture_kind : "")
        }
        onModelChanged: {
            right_side.syncComboToValue(apertureTypeCombo,
                                        right_side.groupEditor ? right_side.groupEditor.aperture_kind : "")
        }
        onActivated: {
            if (right_side.groupEditor && currentText.length > 0) {
                right_side.groupEditor.aperture_kind = currentText
            }
        }
        
        Connections {
            target: right_side.groupEditor
            function onKind_changed() {
                right_side.syncComboToValue(apertureTypeCombo,
                                            right_side.groupEditor ? right_side.groupEditor.aperture_kind : "")
            }
        }
    }
    
    PropertyLabel {
        text: "Surface Type"
    }
    
    GlassComboBox {
        id: surfaceTypeCombo
        Layout.fillWidth: true
        model: right_side.groupEditor ? right_side.groupEditor.surface_type_model : null
        textRole: "display"
        enabled: count > 0
        onCountChanged: {
            right_side.syncComboToValue(surfaceTypeCombo,
                                        right_side.groupEditor ? right_side.groupEditor.surface_kind : "")
        }
        onModelChanged: {
            right_side.syncComboToValue(surfaceTypeCombo,
                                        right_side.groupEditor ? right_side.groupEditor.surface_kind : "")
        }
        onActivated: {
            if (!right_side.groupEditor || currentText.length === 0) {
                return
            }
            if (right_side.groupEditor.surface_kind !== currentText) {
                right_side.groupEditor.surface_kind = currentText
                right_side.groupEditor.surface_arguments =
                        right_side.defaultSurfaceArguments(currentText)
            }
        }
        
        Connections {
            target: right_side.groupEditor
            function onSurface_kind_changed() {
                right_side.syncComboToValue(surfaceTypeCombo,
                                            right_side.groupEditor ? right_side.groupEditor.surface_kind : "")
            }
        }
    }
    
    PropertyLabel {
        text: right_side.surfaceArgLabel(0)
        visible: right_side.surfaceArgCount() > 0
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.surfaceArgCount() > 0
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.surfaceArgValue(0))
        onRealValueModified: right_side.setSurfaceArg(0, realValue)
    }
    
    PropertyLabel {
        text: right_side.surfaceArgLabel(1)
        visible: right_side.surfaceArgCount() > 1
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.surfaceArgCount() > 1
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.surfaceArgValue(1))
        onRealValueModified: right_side.setSurfaceArg(1, realValue)
    }
    
    PropertyLabel {
        text: right_side.surfaceArgLabel(2)
        visible: right_side.surfaceArgCount() > 2
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.surfaceArgCount() > 2
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.surfaceArgValue(2))
        onRealValueModified: right_side.setSurfaceArg(2, realValue)
    }
    
    PropertyLabel {
        text: right_side.surfaceArgLabel(3)
        visible: right_side.surfaceArgCount() > 3
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.surfaceArgCount() > 3
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.surfaceArgValue(3))
        onRealValueModified: right_side.setSurfaceArg(3, realValue)
    }
}
