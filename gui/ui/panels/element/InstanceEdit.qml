import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

STPropertyPanel {
    id: root
    property var module : App.layout.instance_edit

    STPropertyLabel {
        text: "Material"
    }

    STButton {
        Layout.fillWidth: true
        property string material_name: module.current_material_name
        text: material_name.length ? material_name : "Unassigned"

        onClicked: material_pop.open()

        SelectItemPopup {
            id: material_pop
            source_model: AppData.materials.materials_list

            onSelectedEntity: (entity) => module.current_material = entity
        }
    }

    STPropertyLabel {
        text: "Geometry"
    }

    STButton {
        Layout.fillWidth: true
        property string geometry_name: module.current_geometry_name
        text: geometry_name.length ? geometry_name : "Unassigned"

        onClicked: geometry_pop.open()

        SelectItemPopup {
            id: geometry_pop
            source_model: AppData.materials.geometry_list

            onSelectedEntity: (entity) => module.current_geometry = entity
        }
    }

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
        id: rotation_panel
        Layout.columnSpan: 2
        Layout.fillWidth: true

        title: "Parent-relative Rotation"
        collapsible: true

        property vector3d angles: module.orientation.toEulerAngles()

        STPropertyLabel {
            text: "X Angle"
        }

        STTextField {
            id: x_euler
            Layout.fillWidth: true
            text: rotation_panel.angles.x

            validator: DoubleValidator {}

            onAccepted: rotation_panel.update_from_angles()
        }

        STPropertyLabel {
            text: "Y Angle"
        }

        STTextField {
            id: y_euler
            Layout.fillWidth: true
            text: rotation_panel.angles.y

            validator: DoubleValidator {}

            onAccepted: rotation_panel.update_from_angles()
        }

        STPropertyLabel {
            text: "Z Angle"
        }

        STTextField {
            id: z_euler
            Layout.fillWidth: true
            text: rotation_panel.angles.z

            validator: DoubleValidator {}

            onAccepted: rotation_panel.update_from_angles()
        }

        function update_from_angles() {
            root.module.set_from_angles(
                        Qt.vector3d(x_euler.text, y_euler.text, z_euler.text)
                        )
        }

    }

    CheckBoxField {
        text: "Hidden"
        value: module.hidden
        Layout.fillWidth: true
        Layout.columnSpan: 2

        onClicked: module.hidden = !module.hidden
    }

    CheckBoxField {
        text: "Disabled"
        value: module.disabled
        Layout.fillWidth: true
        Layout.columnSpan: 2

        onClicked: module.disabled = !module.disabled
    }



}
