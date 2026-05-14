import QtQuick
import QtQuick.Controls.Material
import SolTrace

DoubleSpinBox {
    id: control
    editable: true

    property string suffix

    property string internal_computed_suffix : suffix.length ? " " + suffix : ""

    textFromValue: function(value, locale) {
        return Number(value).toLocaleString(locale, 'f', control.decimals) + internal_computed_suffix
    }

    valueFromText: function(text, locale) {
        if (suffix.length) {
            text = text.replace(suffix,"")
        }

        return Number.fromLocaleString(locale, text)
    }

    contentItem: TextInput {
        z: 2
        text: control.textFromValue(control.value, control.locale)
        color: App.theme.fontColor
        font.family: control.font.family
        font.pointSize: App.theme.labelSize
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
    }

    background: WellRectangle {
        implicitWidth: 80
        implicitHeight: 32
        radius: height / 2
    }

    up.indicator: Rectangle {
        x: control.mirrored ? 0 : parent.width - width
        height: parent.height
        implicitWidth: 32
        implicitHeight: 32
        color: control.up.pressed ? Qt.rgba(1, 1, 1, 0.1) : "transparent"
        radius: height / 2
        opacity: (control.hovered || control.activeFocus) ? 1 : 0

        Label {
            text: "\u002b"
            font.family: "Font Awesome 7 Free"
            font.pointSize: control.font.pointSize
            anchors.centerIn: parent
        }
    }

    down.indicator: Rectangle {
        x: control.mirrored ? parent.width - width : 0
        height: parent.height
        implicitWidth: 32
        implicitHeight: 32
        color: control.down.pressed ? Qt.rgba(1, 1, 1, 0.1) : "transparent"
        radius: height / 2
        opacity: (control.hovered || control.activeFocus) ? 1 : 0

        Label {
            text: "\uf068"
            font.family: "Font Awesome 7 Free"
            font.pointSize: control.font.pointSize
            anchors.centerIn: parent
        }
    }
}
