import QtQuick
import QtQuick.Controls.Basic
import SolTraceProto

Rectangle {
    id: field
    color: backgroundColor
    border.width: 1
    border.color: Theme.lineColor

    property real value: 0.0
    property string textValue: ""
    property real minValue: 0.0
    property real maxValue: 1.0
    property color backgroundColor: "transparent"

    // Flag for Angular Reflectance and Angular Transmittance functions
    property bool isFunction: false

    Text {
        anchors.centerIn: parent
        text: "—" // Placeholder text
        color: Qt.rgba(Theme.textColor.r, Theme.textColor.g, Theme.textColor.b, 0.5)
        font.pixelSize: 16
        font.bold: true
        visible: field.isFunction && !textFieldItem.activeFocus && textFieldItem.text === ""
    }

    TextField {
        id: textFieldItem
        anchors.centerIn: parent
        width: parent.width - 10
        height: parent.height - 10
        text: field.isFunction ? field.textValue : field.value.toFixed(4)
        horizontalAlignment: Text.AlignHCenter

        background: Rectangle { color: "transparent" }
        color: Theme.textColor
        selectByMouse: true

        onEditingFinished: {
            if (field.isFunction) {
                field.textValueChanged(text)
            } else {
                let val = parseFloat(text)
                if (!isNaN(val)) {
                    field.valueChanged(Math.max(field.minValue, Math.min(field.maxValue, val)))
                }
            }
        }
    }
}
