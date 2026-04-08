import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root

    Label {
        text: "Location"
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

            label: "Latitude"

            from: -90
            to: 90
            decimals: 4
        }

        STSpinBoxField {
            Layout.row: App.view.left_panel.size == PanelData.Small ? 1 : 0
            Layout.column: App.view.left_panel.size == PanelData.Small ? 0 : 1
            Layout.preferredWidth: 200

            label: "Longitude"

            from: -180
            to: 180
            decimals: 4
        }
    }
}
