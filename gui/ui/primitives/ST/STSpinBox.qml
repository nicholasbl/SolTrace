import QtQuick 2.15
import QtQuick.Controls.Material
import SolTrace

SpinBox {
    id: control
    editable: true
    from: 0
    to: 99
    stepSize: decimalFactor

    property int decimals: 0
    property real realValue: value / decimalFactor
    readonly property int decimalFactor: Math.pow(10, decimals)

    property string suffix

    property string internal_computed_suffix : suffix.length ? " " + suffix : ""

    signal realValueModified(value: real)

    onValueModified: realValueModified(realValue)

    function decimalToInt(decimal) {
        return decimal * decimalFactor
    }

    validator: DoubleValidator {
        bottom: Math.min(control.from, control.to) / control.decimalFactor
        top: Math.max(control.from, control.to) / control.decimalFactor
        decimals: control.decimals
        notation: DoubleValidator.StandardNotation
    }

    textFromValue: function(value, locale) {
        return Number(value / decimalFactor).toLocaleString(locale, 'f', control.decimals) + internal_computed_suffix
    }

    valueFromText: function(text, locale) {
        if (suffix.length) {
            text = text.replace(suffix,"")
        }

        return Math.round(Number.fromLocaleString(locale, text) * decimalFactor)
    }

    contentItem: TextInput {
        z: 2
        color: Material.foreground
        text: control.textFromValue(control.value, control.locale)
        font: control.font
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
            font.pixelSize: control.font.pixelSize
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
            font.pixelSize: control.font.pixelSize
            anchors.centerIn: parent
        }
    }
}
