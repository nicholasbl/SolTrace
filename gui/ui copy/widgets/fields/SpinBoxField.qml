import QtQuick 2.15
import QtQuick.Controls
import SolTraceProto

Column {
    id: root
    spacing: 5

    property string label: ""
    property string labelSize: Theme.controlLabelSize
    property real minValue: 0
    property real maxValue: 100
    property real stepSize: 1
    property string placeholder: ""
    property int fieldWidth: 80
    property int decimals: 0

    property alias value: spinBox.value
    property alias realValue: spinBox.realValue
    property alias spinBox: spinBox

    function decimalToInt(decimal) {
        return spinBox.decimalToInt(decimal)
    }

    function setValue(newValue) {
        spinBox.value = spinBox.decimalToInt(newValue)
    }

    GlassSpinBox {
        id: spinBox
        width: root.fieldWidth
        from: decimalToInt(root.minValue)
        to: decimalToInt(root.maxValue)
        stepSize: decimalToInt(root.stepSize)
        decimals: root.decimals
    }

    STLabel {
        text: root.label
        visible: root.label !== ""
        font.pointSize: root.labelSize
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
