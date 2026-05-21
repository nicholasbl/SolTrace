import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root

    STComboBar {
        id: bar

        currentIndex: App.view.analyze_section
        onCurrentIndexChanged: App.view.analyze_section = currentIndex

        Layout.fillWidth: true

        collapseLabels: App.view.left_panel.size === PanelData.Small

        iconModel: ["\uf1da", "\uf601", "\uf201"]
        model: ["History", "Intersections", "Flux"]

    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Material.dividerColor
    }

    StackLayout {
        currentIndex: App.view.analyze_section
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: 8

        HistoryPanel {

        }

        AnalyzeIntersections {

        }

        AnalyzeFlux {

        }
    }
}
