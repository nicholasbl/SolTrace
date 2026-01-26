import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto
Column {
    DocSection {
        width: parent.width
        header.text: Documentation.sun.headers.spaCalculator
        body.text: Documentation.sun.spaCalculator
    }
    Row {
        x: !UserSettings.showDocumentation ? 0 : (parent.width - width) / 2
        spacing: 20
        LocationField {
            latitude: Session.core.sun.directionalSun.latitude
            longitude: Session.core.sun.directionalSun.longitude
            onLatitudeChanged: function (newLatitude) {
                if (newLatitude) Session.core.sun.directionalSun.latitude = newLatitude
            }
            onLongitudeChanged: function (newLongitude) {
                if (newLongitude) Session.core.sun.directionalSun.longitude = newLongitude
            }
        }
        DateField {
            year: Session.core.sun.directionalSun.year
            month: Session.core.sun.directionalSun.month
            day: Session.core.sun.directionalSun.day
            onYearChanged: function (newYear) {
                if (newYear) Session.core.sun.directionalSun.year = newYear
            }
            onMonthChanged: function (newMonth) {
                if (newMonth) Session.core.sun.directionalSun.month = newMonth
            }
            onDayChanged: function (newDay) {
                if (newDay) Session.core.sun.directionalSun.day = newDay
            }
        }
        TimeField {
            showTimezone: true
            hour: Session.core.sun.directionalSun.hour
            minute: Session.core.sun.directionalSun.minute
            second: Session.core.sun.directionalSun.second
            timezone: Session.core.sun.directionalSun.timezone
            onHourChanged: function (newHour) {
                if (newHour) Session.core.sun.directionalSun.hour = newHour
            }
            onMinuteChanged: function (newMinute) {
                if (newMinute) Session.core.sun.directionalSun.minute = newMinute
            }
            onSecondChanged: function (newSecond) {
                if (newSecond) Session.core.sun.directionalSun.second = newSecond
            }
            onTimezoneChanged: function (newTimezone) {
                if (newTimezone) Session.core.sun.directionalSun.timezone = newTimezone
            }
        }
    }
    Spacer {}
    Spacer {}
    Spacer {}
    Spacer {}
    Column {
        spacing: 10
        x: !UserSettings.showDocumentation ? 0 : (parent.width - width) / 2
        CheckBoxField {
            id: optionalFieldCheckBox
            label: "Optional fields"
            checked: Session.core.sun.directionalSun.optionalFieldsEnabled
            onCheckBoxClicked: {
                Session.core.sun.directionalSun.optionalFieldsEnabled = !Session.core.sun.directionalSun.optionalFieldsEnabled
            }
        }

        Row {
            opacity: Session.core.sun.directionalSun.optionalFieldsEnabled ? 1 : 0.5
            spacing: 20
            NumberField {
                unit: "s"
                unitLabel: "ΔUT1"
                fieldWidth: 150
                minValue: -1
                value: Session.core.sun.directionalSun.deltaUt1
                maxValue: 1
                decimals: 3
                enforceMin: true
                enforceMax: true
                onValueChanged: {
                    Session.core.sun.directionalSun.deltaUt1 = value
                }
            }
            NumberField {
                unit: "m"
                unitLabel: "Altitude"
                fieldWidth: 150
                value: Session.core.sun.directionalSun.altitude
                minValue: -6500000
                decimals: 3
                enforceMin: true
                enforceMax: false
                onValueChanged: {
                    Session.core.sun.directionalSun.altitude = value
                }
            }
            NumberField {
                unit: "mbar"
                unitLabel: "Pressure"
                fieldWidth: 150
                minValue: 0
                maxValue: 5000
                value: Session.core.sun.directionalSun.pressure
                decimals: 3
                enforceMin: true
                enforceMax: true
                onValueChanged: {
                    Session.core.sun.directionalSun.pressure = value
                }
            }
            NumberField {
                unit: "°C"
                unitLabel: "Temperature"
                fieldWidth: 150
                minValue: -273
                maxValue: 6000
                value: Session.core.sun.directionalSun.temperature
                decimals: 3
                enforceMin: true
                enforceMax: true
                onValueChanged: {
                    Session.core.sun.directionalSun.temperature = value
                }
            }
        }
    }
}
