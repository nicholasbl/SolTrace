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

        fontFamily: App.view.left_panel_size == ViewModule.Small ? "Font Awesome 7 Free" : ""

        model : App.view.left_panel_size == ViewModule.Small ?
                    ["\uf06a", "\uf06a"]
                  :
                    ["Flux", "Intersections"]

    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Material.dividerColor
    }
}
