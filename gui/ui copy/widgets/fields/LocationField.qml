import QtQuick 2.15
import QtQuick.Controls
import SolTraceProto

Column {
    id: root
    spacing: 5

    property string label: "Location"
    property bool showLongitude: true
    property alias latitude: latitudeField.value
    property alias longitude: longitudeField.value

    STLabel {
        text: root.label
        visible: root.label !== ""
        font.pointSize: Theme.controlRowLabelSize
    }

    Row {
        spacing: 10

        NumberField {
            id: latitudeField
            unit: "°"
            unitLabel: "Latitude"
            minValue: -90
            maxValue: 90
            decimals: 4
            value: 0
            fieldWidth: 150
        }

        NumberField {
            id: longitudeField
            unit: "°"
            unitLabel: "Longitude"
            minValue: -180
            maxValue: 180
            decimals: 4
            value: 0
            fieldWidth: 150
            visible: root.showLongitude
        }
    }
}
