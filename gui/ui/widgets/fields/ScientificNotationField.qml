import QtQuick 2.15
import QtQuick.Controls
import SolTraceProto

Column {
    id: root
    spacing: 5

    property string label: ""
    property real value: 2e10
    property int mantissaDecimals: 2

    // Constraints
    property real minValue: 1e1
    property real maxValue: 9.99e8

    Body {
        width: parent.width
        rawText: root.label
        visible: root.label !== ""
        font.pointSize: Theme.controlRowLabelSize
    }

    GlassTextField {
        id: textField
        width: 150
        text: root.value.toExponential(root.mantissaDecimals)

        validator: RegularExpressionValidator {
            regularExpression: /^[0-9]*\.?[0-9]+[eE][+-]?[0-9]+$/
        }

        onEditingFinished: {
            var newValue = parseFloat(text)
            if (!isNaN(newValue)) {
                // Clamp to min/max
                newValue = Math.max(root.minValue, Math.min(root.maxValue, newValue))
                root.value = newValue
                text = newValue.toExponential(root.mantissaDecimals)
                root.valueChanged(newValue)
            } else {
                // Reset to current value if invalid
                text = root.value.toExponential(root.mantissaDecimals)
            }
        }

        // Update display when value changes externally
        Connections {
            target: root
            function onValueChanged() {
                if (!textField.activeFocus) {
                    textField.text = root.value.toExponential(root.mantissaDecimals)
                }
            }
        }
    }
}
