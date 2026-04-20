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

        fontFamily: App.view.left_panel.size === PanelData.Small ? "Font Awesome 7 Free" : ""

        model : App.view.left_panel.size === PanelData.Small ?
                    ["\uf06a", "\uf06a"]
                  :
                    ["Intersections", "Flux"]

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

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            InlineDocumentation {
                key: "analyze.intersections"
                target: App.view.left_panel
            }
        }

        AnalyzeFlux {

        }
    }
}
