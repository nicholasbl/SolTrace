import QtQuick 2.15
import QtQuick.Controls
import SolTraceProto
Column {
    id: root
    spacing: 5
    property string label: ""
    property string unit: ""
    property string unitLabel: ""
    property real minValue: 0
    property real maxValue: 100
    property string placeholder: ""
    property int fieldWidth: 80
    property real value: minValue
    property int decimals: 0
    property bool enforceMin: false
    property bool enforceMax: false
    property alias text: textField.text
    property alias textField: textField
    Component.onCompleted: {
        textField.text = root.value.toFixed(root.decimals)
    }
    onValueChanged: {
        if (!textField.activeFocus) {
            textField.text = root.value.toFixed(root.decimals)
        }
    }
    Body {
        width: root.fieldWidth
        rawText: root.label
        visible: root.label !== ""
        font.pointSize: Theme.controlRowLabelSize
    }
    Item {
        width: root.fieldWidth
        height: textField.height
        GlassTextField {
            id: textField
            width: parent.width
            inputMethodHints: root.decimals > 0 ? Qt.ImhFormattedNumbersOnly : Qt.ImhDigitsOnly
            placeholderText: root.placeholder
            rightPadding: unitText.visible ? unitText.width - 4 : 6
            validator: (root.enforceMin || root.enforceMax) ? doubleValidator : null
            height: 40

            DoubleValidator {
                id: doubleValidator
                bottom: root.enforceMin ? root.minValue : -Infinity
                top: root.enforceMax ? root.maxValue : Infinity
                decimals: root.decimals
                notation: DoubleValidator.StandardNotation
            }
            onTextChanged: {
                if (text === "" || text === "-" || text === ".") {
                    return  // Allow empty, negative sign, or decimal point
                }
                var num = parseFloat(text)
                if (!isNaN(num)) {
                    // Clamp to range based on enforceMin/enforceMax
                    if (root.enforceMax && num > root.maxValue) {
                        text = root.maxValue.toFixed(root.decimals)
                        num = root.maxValue
                    } else if (root.enforceMin && num < root.minValue && text.length > String(root.minValue).length) {
                        text = root.minValue.toFixed(root.decimals)
                        num = root.minValue
                    }
                    // Update value
                    root.value = num
                }
            }
            onEditingFinished: {
                // Ensure valid value on blur
                if (text === "" || text === "-" || text === ".") {
                    var defaultValue = root.enforceMin ? root.minValue : 0
                    text = defaultValue.toFixed(root.decimals)
                    root.value = defaultValue
                } else {
                    // Format to correct decimal places
                    var num = parseFloat(text)
                    if (!isNaN(num)) {
                        // Clamp based on enforceMin/enforceMax
                        if (root.enforceMin) {
                            num = Math.max(root.minValue, num)
                        }
                        if (root.enforceMax) {
                            num = Math.min(root.maxValue, num)
                        }
                        text = num.toFixed(root.decimals)
                        root.value = num
                    }
                }
            }
        }
        Body {
            id: unitText
            text: root.unit
            visible: root.unit !== ""
            color: Theme.textColor
            font: textField.font
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    Label {
        text: root.unitLabel
        visible: root.unitLabel !== ""
        font.pointSize: Theme.controlLabelSize
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
