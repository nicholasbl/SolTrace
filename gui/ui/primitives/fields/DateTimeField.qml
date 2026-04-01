import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTrace

ColumnLayout {
    id: root

    Label {
        text: "Date"
        font.pointSize: 14
    }

    GridLayout {
        rows: 2
        columns: 2
        Layout.fillWidth: true

        STSpinBoxField {
            Layout.row: 0
            Layout.column: 0
            Layout.preferredWidth: 200

            label: "Month"
            from: 1
            to: 12
            stepSize: 1
        }

        STSpinBoxField {
            Layout.row: App.view.left_panel_size == ViewModule.Small ? 1 : 0
            Layout.column: App.view.left_panel_size == ViewModule.Small ? 0 : 1
            Layout.preferredWidth: 200

            label: "Day"
            from: 1
            to: 31
            stepSize: 1
        }
    }

    Label {
        text: "Time"
        font.pointSize: 14
    }

    GridLayout {
        rows: 2
        columns: 2
        Layout.fillWidth: true

        STSpinBoxField {
            Layout.row: 0
            Layout.column: 0
            Layout.preferredWidth: 200

            label: "Hour"
            from: 0
            to: 23
            stepSize: 1
        }

        STSpinBoxField {
            Layout.row: App.view.left_panel_size == ViewModule.Small ? 1 : 0
            Layout.column: App.view.left_panel_size == ViewModule.Small ? 0 : 1
            Layout.preferredWidth: 200

            label: "Minute"
            from: 0
            to: 59
            stepSize: 1
        }
    }
}
