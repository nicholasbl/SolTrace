import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto
Column {
    DocSection {
        width: parent.width
        header.text: Documentation.sun.headers.duffieCalculator
        body.text: Documentation.sun.duffieCalculator
    }
    Row {
        spacing: 20
        x: !UserSettings.showDocumentation ? 0 : (parent.width - width) / 2
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
            showTimezone: true
            hour: Session.core.sun.directionalSun.hour
            minute: Session.core.sun.directionalSun.minute
            timezone: Session.core.sun.directionalSun.timezone
            onHourChanged: function (newHour) {
                if (newHour) Session.core.sun.directionalSun.hour = newHour
            }
            onMinuteChanged: function (newMinute) {
                if (newMinute) Session.core.sun.directionalSun.minute = newMinute
            }
            onTimezoneChanged: function (newTimezone) {
                if (newTimezone) Session.core.sun.directionalSun.timezone = newTimezone
            }
        }
    }
}
