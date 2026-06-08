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

        currentIndex: Math.min(App.view.analyze_section, 1)
        onCurrentIndexChanged: App.view.analyze_section = currentIndex

        Layout.fillWidth: true

        collapseLabels: App.view.left_panel.size === PanelData.Small

        iconModel: ["\uf601", "\uf201"]
        model: ["Intersections", "Flux"]

    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Material.dividerColor
    }

    StackLayout {
        currentIndex: bar.currentIndex
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: 8

        AnalyzeIntersections {

        }

        AnalyzeFlux {

        }
    }
}
