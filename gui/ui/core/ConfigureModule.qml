import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root
    property int size_class

    STComboBar {
        id: bar
        
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
        currentIndex: bar.currentIndex
        
        ColumnLayout {
            STSpinBox {
                Layout.fillWidth: true
                from: decimalToInt(0)
                to: decimalToInt(1000)
                decimals: 3
                stepSize: decimalToInt(0.01)
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
        }

        Item {

        }
    }
    
}
