import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto
Column {
    DocSection {
        width: parent.width
        header.text: Documentation.sun.headers.legacyCalculator
        body.text: Documentation.sun.legacyCalculator
    }
    Row {
        spacing: 20
        x: !Settings.showDocumentation ? 0 : (parent.width - width) / 2
        LocationField {
            showLongitude: false
            latitude: Session.core.sun.directionalSun.latitude
            onLatitudeChanged: function (newLatitude) {
                if (newLatitude) Session.core.sun.directionalSun.latitude = newLatitude
            }
        }
        DateField {
            showYear: false
            month: Session.core.sun.directionalSun.month
            day: Session.core.sun.directionalSun.day
            onMonthChanged: function (newMonth) {
                if (newMonth) Session.core.sun.directionalSun.month = newMonth
            }
            onDayChanged: function (newDay) {
                if (newDay) Session.core.sun.directionalSun.day = newDay
            }
        }
        TimeField {
            showSeconds: false
            hour: Session.core.sun.directionalSun.hour
            minute: Session.core.sun.directionalSun.minute
            onHourChanged: function (newHour) {
                if (newHour) Session.core.sun.directionalSun.hour = newHour
            }
            onMinuteChanged: function (newMinute) {
                if (newMinute) Session.core.sun.directionalSun.minute = newMinute
            }
        }
    }
}
