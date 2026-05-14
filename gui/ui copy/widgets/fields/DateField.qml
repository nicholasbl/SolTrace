import QtQuick 2.15
import QtQuick.Controls
import SolTraceProto

Column {
    id: root
    spacing: 5

    property string label: "Date"
    property bool showYear: true
    property alias year: yearSpinBox.value
    property alias month: monthSpinBox.value
    property alias day: daySpinBox.value

    Label {
        text: root.label
        visible: root.label !== ""
        font.pointSize: Theme.controlRowLabelSize
    }

    Row {
        spacing: 10

        Column {
            spacing: 5
            visible: root.showYear

            GlassSpinBox {
                id: yearSpinBox
                from: 1900
                to: 2100
                value: 2024
                editable: true

                textFromValue: function(value) {
                    return value.toString()
                }

                valueFromText: function(text) {
                    return parseInt(text)
                }
            }

            Label {
                text: "Year"
                font.pointSize: Theme.controlLabelSize
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Column {
            spacing: 5

            GlassSpinBox {
                id: monthSpinBox
                from: 1
                to: 12
                value: 1
                editable: true

                textFromValue: function(value) {
                    return value.toString().padStart(2, '0')
                }
            }

            Label {
                text: "Month"
                font.pointSize: Theme.controlLabelSize
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Column {
            spacing: 5

            GlassSpinBox {
                id: daySpinBox
                from: 1
                to: 31
                value: 1
                editable: true

                textFromValue: function(value) {
                    return value.toString().padStart(2, '0')
                }
            }

            Label {
                text: "Day"
                font.pointSize: Theme.controlLabelSize
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
