import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

PropertyPanel {
    title: "Aperture Parameters"
    collapsed: false
    enabled: right_side.hasSelection && right_side.apertureEditor
    
    PropertyLabel {
        text: "Inner Radius"
        visible: right_side.hasProp(right_side.apertureEditor, "inner_radius")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "inner_radius")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.inner_radius : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.inner_radius = realValue
        }
    }
    
    PropertyLabel {
        text: "Outer Radius"
        visible: right_side.hasProp(right_side.apertureEditor, "outer_radius")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "outer_radius")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.outer_radius : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.outer_radius = realValue
        }
    }
    
    PropertyLabel {
        text: "Arc Angle"
        visible: right_side.hasProp(right_side.apertureEditor, "arc_angle")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "arc_angle")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.arc_angle : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.arc_angle = realValue
        }
    }
    
    PropertyLabel {
        text: "Diameter"
        visible: right_side.hasProp(right_side.apertureEditor, "diameter")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "diameter")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.diameter : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.diameter = realValue
        }
    }
    
    PropertyLabel {
        text: "Circumscribe Diameter"
        visible: right_side.hasProp(right_side.apertureEditor, "circumscribe_diameter")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "circumscribe_diameter")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.circumscribe_diameter : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.circumscribe_diameter = realValue
        }
    }
    
    PropertyLabel {
        text: "Rect X"
        visible: right_side.hasProp(right_side.apertureEditor, "rectangle")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "rectangle")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(
                   right_side.apertureEditor && right_side.apertureEditor.rectangle
                   ? right_side.apertureEditor.rectangle.x : 0)
        onRealValueModified: right_side.setRectangleField("x", realValue)
    }
    
    PropertyLabel {
        text: "Rect Y"
        visible: right_side.hasProp(right_side.apertureEditor, "rectangle")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "rectangle")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(
                   right_side.apertureEditor && right_side.apertureEditor.rectangle
                   ? right_side.apertureEditor.rectangle.y : 0)
        onRealValueModified: right_side.setRectangleField("y", realValue)
    }
    
    PropertyLabel {
        text: "Rect Width"
        visible: right_side.hasProp(right_side.apertureEditor, "rectangle")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "rectangle")
        decimals: 4
        from: decimalToInt(0)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(
                   right_side.apertureEditor && right_side.apertureEditor.rectangle
                   ? right_side.apertureEditor.rectangle.width : 0)
        onRealValueModified: right_side.setRectangleField("width", realValue)
    }
    
    PropertyLabel {
        text: "Rect Height"
        visible: right_side.hasProp(right_side.apertureEditor, "rectangle")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "rectangle")
        decimals: 4
        from: decimalToInt(0)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(
                   right_side.apertureEditor && right_side.apertureEditor.rectangle
                   ? right_side.apertureEditor.rectangle.height : 0)
        onRealValueModified: right_side.setRectangleField("height", realValue)
    }
    
    PropertyLabel {
        text: "X1"
        visible: right_side.hasProp(right_side.apertureEditor, "x1")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "x1")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.x1 : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.x1 = realValue
        }
    }
    
    PropertyLabel {
        text: "Y1"
        visible: right_side.hasProp(right_side.apertureEditor, "y1")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "y1")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.y1 : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.y1 = realValue
        }
    }
    
    PropertyLabel {
        text: "X2"
        visible: right_side.hasProp(right_side.apertureEditor, "x2")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "x2")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.x2 : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.x2 = realValue
        }
    }
    
    PropertyLabel {
        text: "Y2"
        visible: right_side.hasProp(right_side.apertureEditor, "y2")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "y2")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.y2 : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.y2 = realValue
        }
    }
    
    PropertyLabel {
        text: "X3"
        visible: right_side.hasProp(right_side.apertureEditor, "x3")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "x3")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.x3 : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.x3 = realValue
        }
    }
    
    PropertyLabel {
        text: "Y3"
        visible: right_side.hasProp(right_side.apertureEditor, "y3")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "y3")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.y3 : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.y3 = realValue
        }
    }
    
    PropertyLabel {
        text: "X4"
        visible: right_side.hasProp(right_side.apertureEditor, "x4")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "x4")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.x4 : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.x4 = realValue
        }
    }
    
    PropertyLabel {
        text: "Y4"
        visible: right_side.hasProp(right_side.apertureEditor, "y4")
    }
    
    GlassSpinBox {
        Layout.fillWidth: true
        visible: right_side.hasProp(right_side.apertureEditor, "y4")
        decimals: 4
        from: decimalToInt(-100000)
        to: decimalToInt(100000)
        stepSize: decimalToInt(0.01)
        value: decimalToInt(right_side.apertureEditor ? right_side.apertureEditor.y4 : 0)
        onRealValueModified: {
            if (right_side.apertureEditor) right_side.apertureEditor.y4 = realValue
        }
    }
}
