import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto
Column {
    DocSection {
        width: parent.width
        header.text: Documentation.sun.headers.solposCalculator
        body.text: Documentation.sun.solposCalculator
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
    }
    Spacer {}
    Spacer {}
    Row {
        spacing: 10
        x: !UserSettings.showDocumentation ? 0 : (parent.width - width) / 2
        TimeField {
            label: "Time"
            showSeconds: true
            hour: Session.core.sun.directionalSun.hour
            minute: Session.core.sun.directionalSun.minute
            second: Session.core.sun.directionalSun.second
            onHourChanged: function (newHour) {
                if (newHour) Session.core.sun.directionalSun.hour = newHour
            }
            onMinuteChanged: function (newMinute) {
                if (newMinute) Session.core.sun.directionalSun.minute = newMinute
            }
            onSecondChanged: function (newSecond) {
                if (newSecond) Session.core.sun.directionalSun.second = newSecond
            }
        }
        Column {
            spacing: 5
            anchors.bottom: parent.bottom
            GlassComboBox {
                width: 80
                model: ["1", "2", "5", "10", "15", "30", "60"]
                currentIndex: {
                    var interval = Session.core.sun.directionalSun.interval
                    var intervals = [1, 2, 5, 10, 15, 30, 60]
                    var index = intervals.indexOf(interval)
                    return index !== -1 ? index : 0
                }
                //borderColor: Theme.lineColor
                onCurrentIndexChanged: {
                    var intervals = [1, 2, 5, 10, 15, 30, 60]
                    Session.core.sun.directionalSun.interval = intervals[currentIndex]
                }
            }
            STLabel {
                text: "Interval (sec)"
                font.pointSize: Theme.controlLabelSize
            }
        }
    }
}
