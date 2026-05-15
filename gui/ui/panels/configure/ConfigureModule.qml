import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root

    Binding {
        target: bar
        property: "currentIndex"
        value: App.view.configure_section
    }

    STComboBar {
        id: bar
        currentIndex: App.view.configure_section
        onCurrentIndexChanged: App.view.configure_section = currentIndex

        Layout.fillWidth: true

        fontFamily: App.view.left_panel.size === PanelData.Small ? "Font Awesome 7 Free" : ""

        model: App.view.left_panel.size === PanelData.Small ?
            ["\uf185", "\uf53f", "\uf1b2", "\ue4e6", "\uf5c3"]
          :
            ["Sun", "Material", "Geometry", "Layout", "Appearance"]
        
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: App.theme.dividerColor
    }
    
    StackLayout {
        currentIndex: App.view.configure_section

        ConfigureSun {
        }

        ConfigureMaterials {
        }
        
        ConfigureGeometry {
        }

        ConfigureLayout {
        }

        ConfigureAppearance {
        }
    }
    
}
