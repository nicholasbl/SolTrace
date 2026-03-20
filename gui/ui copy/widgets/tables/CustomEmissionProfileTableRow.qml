import QtQuick
import QtQuick.Controls.Basic
import SolTraceProto

Row {
    id: root
    height: 40
    spacing: 0

    property int rowNumber: 1
    property real angleValue: 0
    property real intensityValue: 0
    property bool canDelete: true
    property var table: null

    signal angleChanged(real newValue)
    signal intensityChanged(real newValue)
    signal removeClicked()

    // Row number
    Rectangle {
        width: 60
        height: parent.height
        color: "transparent"
        border.width: 1
        border.color: Theme.lineColor

        Text {
            anchors.centerIn: parent
            text: root.rowNumber
            color: Theme.textColor
        }
    }

    // Angle field
    Rectangle {
        width: 170
        height: parent.height
        color: "transparent"
        border.width: 1
        border.color: Theme.lineColor

        TextField {
            id: angleField
            anchors.centerIn: parent
            width: parent.width - 10
            height: parent.height - 10
            text: root.angleValue.toFixed(3)
            horizontalAlignment: Text.AlignHCenter
            background: Rectangle {
                color: "transparent"
            }
            color: Theme.textColor
            selectByMouse: true

            onEditingFinished: {
                let newValue = parseFloat(text)
                if (!isNaN(newValue)) {
                    root.angleChanged(newValue)
                }
            }

            validator: DoubleValidator {
                bottom: -1000
                top: 1000
                decimals: 6
                notation: DoubleValidator.StandardNotation
            }
        }
    }

    // Intensity field
    Rectangle {
        width: 170
        height: parent.height
        color: "transparent"
        border.width: 1
        border.color: Theme.lineColor

        TextField {
            id: intensityField
            anchors.centerIn: parent
            width: parent.width - 10
            height: parent.height - 10
            text: root.intensityValue.toFixed(3)
            horizontalAlignment: Text.AlignHCenter
            background: Rectangle {
                color: "transparent"
            }
            color: Theme.textColor
            selectByMouse: true

            onEditingFinished: {
                let newValue = parseFloat(text)
                if (!isNaN(newValue)) {
                    root.intensityChanged(newValue)
                }
            }

            validator: DoubleValidator {
                bottom: 0
                top: 10
                decimals: 6
                notation: DoubleValidator.StandardNotation
            }
        }
    }

    // Delete button
    Rectangle {
        width: 60
        height: parent.height
        color: "transparent"
        border.width: 1
        border.color: Theme.lineColor

        GlassButton {
            anchors.centerIn: parent
            width: 30
            height: 30
            label: "×"
            enabled: root.canDelete
            opacity: root.canDelete ? 1.0 : 0.3

            onClicked: {
                root.removeClicked()
            }
        }
    }
}
