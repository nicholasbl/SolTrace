import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root
    property int size_class

    Binding {
        target: bar
        property: "currentIndex"
        value: App.view.configure_section
    }

    STComboBar {
        id: bar
        onCurrentIndexChanged: App.view.configure_section = currentIndex

        Layout.fillWidth: true

        fontFamily: size_class < 1 ? "Font Awesome 7 Free" : ""
        
        model : size_class < 1 ?
                    ["\uf0eb", "\uf53f", "\uf1b2", "\ue4e6"]
                  :
                    ["Light", "Material", "Geometry", "Layout"]
        
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Material.dividerColor
    }
    
    StackLayout {
        currentIndex: App.view.configure_section

        ColumnLayout {
            STDoubleSpinBox {
                Layout.fillWidth: true
                from: 0
                to: 1000
                decimals: 3
                stepSize: 0.01
                suffix: "mrad"
            }

            STComboBox {
                Layout.fillWidth: true

                model: ["This is Option A", "Option B", "Option C"]
            }
        }
        
        ConfigureMaterials {
            size_class: root.size_class
        }
        
        ConfigureGeometry {
            size_class: root.size_class
        }

        ConfigureLayout {
            size_class: root.size_class
        }
    }
    
}
