import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

STPropertyPanel {
    property var module : App.layout.instance_edit

    STPropertyPanel {
        id: position_panel
        Layout.columnSpan: 2
        Layout.fillWidth: true

        title: "Parent-relative Position"
        collapsible: true

        STPropertyLabel {
            text: "X"
        }

        // TODO: Replace with targeted control
        // spin boxes dont do the trick here.
        // we want something that you can evaluate (ie 1 + 5)
        STTextField {
            id: x_pos
            Layout.fillWidth: true
            text: module.position.x

            validator: DoubleValidator {}

            onAccepted: position_panel.update_position()
        }

        STPropertyLabel {
            text: "Y"
        }

        STTextField {
            id: y_pos
            Layout.fillWidth: true
            text: module.position.y

            validator: DoubleValidator {}

            onAccepted: position_panel.update_position()
        }

        STPropertyLabel {
            text: "Z"
        }

        STTextField {
            id: z_pos
            Layout.fillWidth: true
            text: module.position.z

            validator: DoubleValidator {}

            onAccepted: position_panel.update_position()
        }

        function update_position() {
            module.position = Qt.vector3d(x_pos.text, y_pos.text, z_pos.text)
        }
    }

    STPropertyPanel {
        Layout.columnSpan: 2
        Layout.fillWidth: true

        title: "Parent-relative Rotation"
        collapsible: true

        STPropertyLabel {
            text: "X"
        }

        STTextField {
            Layout.fillWidth: true
            text: module.orientation.x
        }

        STPropertyLabel {
            text: "Y"
        }

        STTextField {
            Layout.fillWidth: true
            text: module.orientation.y
        }

        STPropertyLabel {
            text: "Z"
        }

        STTextField {
            Layout.fillWidth: true
            text: module.orientation.z
        }

        STPropertyLabel {
            text: "W"
        }

        STTextField {
            Layout.fillWidth: true
            text: module.orientation.scalar
        }

    }

    STPropertyLabel {
        text: "Hidden"
    }

    CheckBox {
        Layout.fillWidth: true
    }

}
