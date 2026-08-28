import QtQuick
import QtQuick.Layouts

import SolTrace

STPropertyPanel {
    id: root

    property var module
    property bool singleColumn: false
    property int labelAlignment: Qt.AlignRight | Qt.AlignVCenter
    property int childColumnSpan: singleColumn ? 1 : 2

    columns: root.singleColumn ? 1 : 2

    STPropertySeparator {
        title: "Placement"
    }

    STSwitch {
        Layout.columnSpan: root.childColumnSpan
        text: "Use 3D widget"
        checked: App.view.mouse_mode === ViewModule.EditElement
        onToggled: App.view.mouse_mode = checked ? ViewModule.EditElement
                                                  : ViewModule.Camera
    }

    InlineDocumentation {
        key: "configure.layout.coordinates"
        Layout.columnSpan: root.childColumnSpan
        Layout.fillWidth: true
    }

    STPropertyLabel {
        text: "Coordinates"
        Layout.alignment: root.labelAlignment
        Layout.columnSpan: 1
    }

    STComboBox {
        id: positionModeCombo
        Layout.columnSpan: 1
        Layout.fillWidth: true
        model: ["Local", "Global"]
    }

    StackLayout {
        id: positionSwipe
        Layout.columnSpan: root.childColumnSpan
        Layout.fillWidth: true
        currentIndex: positionModeCombo.currentIndex

        ElementPositionEditor {
            Layout.preferredWidth: positionSwipe.width
            module: root.module
            singleColumn: root.singleColumn
            labelAlignment: root.labelAlignment
            targetProperty: "position"
            resetText: "Reset Local Default Position"
        }

        ElementPositionEditor {
            Layout.preferredWidth: positionSwipe.width
            module: root.module
            singleColumn: root.singleColumn
            labelAlignment: root.labelAlignment
            targetProperty: "global_position"
            resetText: "Reset Global Default Position"
        }
    }

    ElementRotationEditor {
        Layout.columnSpan: root.childColumnSpan
        Layout.fillWidth: true
        module: root.module
        singleColumn: root.singleColumn
        labelAlignment: root.labelAlignment
    }
}
