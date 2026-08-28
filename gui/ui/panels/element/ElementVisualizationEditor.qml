import QtQuick.Layouts

import SolTrace

STPropertyPanel {
    id: root

    property var module
    property bool singleColumn: false
    property int childColumnSpan: singleColumn ? 1 : 2

    columns: root.singleColumn ? 1 : 2

    ColorPickerField {
        id: elementColorPicker
        Layout.columnSpan: root.childColumnSpan
        Layout.preferredWidth: 200
        color: root.module.color
        label: "Element Tint"
        onUpdated: {
            root.module.color = elementColorPicker.color
        }
    }

    STSwitch {
        Layout.columnSpan: root.childColumnSpan
        text: "Show in 3D View"
        checked: !root.module.hidden
        onToggled: root.module.hidden = !checked
    }

    STSwitch {
        Layout.columnSpan: root.childColumnSpan
        text: "Exclude from Simulation"
        checked: root.module.disabled
        onToggled: root.module.disabled = checked
    }

    STSwitch {
        Layout.columnSpan: root.childColumnSpan
        text: "Ray Probe"
        checked: root.module.virtual_element
        onToggled: root.module.virtual_element = checked
    }
}
