import QtQuick 2.15
import QtQuick.Controls
import SolTraceProto

Column {
    id: root
    spacing: 5

    property string label: "Time"
    property bool showSeconds: false
    property bool showTimezone: false
    property alias hour: hourSpinBox.value
    property alias minute: minuteSpinBox.value
    property alias second: secondSpinBox.value
    property alias timezone: timezoneSpinBox.value

    STLabel {
        text: root.label
        visible: root.label !== ""
        font.pointSize: Theme.controlRowLabelSize
    }

    Row {
        spacing: 10

        Column {
            spacing: 5

            GlassSpinBox {
                id: hourSpinBox
                from: 0
                to: 23
                value: 12
                editable: true

                textFromValue: function(value) {
                    return value.toString().padStart(2, '0')
                }
            }

            STLabel {
                text: "Hour"
                font.pointSize: Theme.controlLabelSize
                anchors.horizontalCenter: parent.horizontalCenter
            }

        }

        Column {
            spacing: 5

            GlassSpinBox {
                id: minuteSpinBox
                from: 0
                to: 59
                value: 0
                editable: true

                textFromValue: function(value) {
                    return value.toString().padStart(2, '0')
                }
            }

            STLabel {
                text: "Minute"
                font.pointSize: Theme.controlLabelSize
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Column {
            spacing: 5
            visible: root.showSeconds

            GlassSpinBox {
                id: secondSpinBox
                from: 0
                to: 59
                value: 0
                editable: true

                textFromValue: function(value) {
                    return value.toString().padStart(2, '0')
                }
            }

            STLabel {
                text: "Second"
                font.pointSize: Theme.controlLabelSize
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Column {
            spacing: 5
            visible: root.showTimezone


            GlassSpinBox {
                id: timezoneSpinBox
                from: -12
                to: 14
                value: 0
                editable: true

                textFromValue: function(value) {
                    return "UTC" + (value >= 0 ? "+" : "") + value.toString()
                }

                valueFromText: function(text) {
                    return parseInt(text.replace("UTC", ""))
                }
            }

            STLabel {
                text: "Timezone"
                font.pointSize: Theme.controlLabelSize
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
