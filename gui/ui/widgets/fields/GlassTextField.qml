import QtQuick 2.15
import QtQuick.Controls.Basic
import SolTraceProto

TextField {
    id: control

    property string placeholder: ""
    property color borderColor: Theme.lineColor
    property color activeFocusBorderColor: Theme.lineColor

    color: Theme.textColor
    selectionColor: Theme.textColor
    selectedTextColor: Theme.accentColor
    placeholderTextColor: Theme.textColor
    placeholderText: placeholder

    font.pointSize: Theme.controlLabelSize
    horizontalAlignment: TextInput.AlignHCenter
    verticalAlignment: TextInput.AlignVCenter

    background: Rectangle {
        implicitWidth: 150
        implicitHeight: control.height
        color: "transparent"
        border.color: control.activeFocus ? control.activeFocusBorderColor : control.borderColor
        border.width: 1
        radius: 8
    }
}
