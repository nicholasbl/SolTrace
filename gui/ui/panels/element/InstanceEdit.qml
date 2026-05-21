import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root

    property var module: App.layout.instance_edit
    property bool singleColumn: App.view.left_panel.size === PanelData.Small
    property var labelAlignment: (singleColumn ? Qt.AlignLeft : Qt.AlignRight) | Qt.AlignVCenter

    STPropertyPanel {
        Layout.fillWidth: true
        columns: 2

        STPropertyLabel {
            text: "Parent"
            Layout.alignment: root.labelAlignment
            Layout.columnSpan: root.singleColumn ? 2 : 1
        }

        STButton {
            Layout.fillWidth: true
            Layout.columnSpan: root.singleColumn ? 2 : 1
            property string parent_name: root.module.parent_name
            text: parent_name.length ? parent_name : "Unassigned"
            onClicked: parent_pop.open()
            SelectEntityPopup {
                id: parent_pop
                exclude: [root.module.entity]
                allowNothing: true
                onSelectedEntity: (entity) => root.module.parent = entity
                onSelectedNothing: root.module.clear_parent()
            }
        }

        STPropertyLabel {
            text: "Material"
            Layout.alignment: root.labelAlignment
            Layout.columnSpan: root.singleColumn ? 2 : 1
        }

        STButton {
            Layout.fillWidth: true
            Layout.columnSpan: root.singleColumn ? 2 : 1
            property string material_name: root.module.current_material_name
            text: material_name.length ? material_name : "Unassigned"
            onClicked: material_pop.open()
            SelectItemPopup {
                id: material_pop
                source_model: AppData.materials.materials_list
                onSelectedEntity: (entity) => root.module.current_material = entity
            }
        }

        STPropertyLabel {
            text: "Geometry"
            Layout.alignment: root.labelAlignment
            Layout.columnSpan: root.singleColumn ? 2 : 1
        }

        STButton {
            Layout.fillWidth: true
            Layout.columnSpan: root.singleColumn ? 2 : 1
            property string geometry_name: root.module.current_geometry_name
            text: geometry_name.length ? geometry_name : "Unassigned"
            onClicked: geometry_pop.open()
            SelectItemPopup {
                id: geometry_pop
                source_model: AppData.materials.geometry_list
                onSelectedEntity: (entity) => root.module.current_geometry = entity
            }
        }

        STPropertyPanel {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            title: "Parent-relative Position"
            collapsible: true

            STPropertyLabel { text: "X" }
            STTextField {
                id: x_pos
                Layout.fillWidth: true
                text: root.module.position.x
                validator: DoubleValidator {}
                onAccepted: root.module.position = Qt.vector3d(x_pos.text, y_pos.text, z_pos.text)
            }

            STPropertyLabel { text: "Y" }
            STTextField {
                id: y_pos
                Layout.fillWidth: true
                text: root.module.position.y
                validator: DoubleValidator {}
                onAccepted: root.module.position = Qt.vector3d(x_pos.text, y_pos.text, z_pos.text)
            }

            STPropertyLabel { text: "Z" }
            STTextField {
                id: z_pos
                Layout.fillWidth: true
                text: root.module.position.z
                validator: DoubleValidator {}
                onAccepted: root.module.position = Qt.vector3d(x_pos.text, y_pos.text, z_pos.text)
            }

            STButton {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: "Reset"
                onClicked: {
                    x_pos.text = 0
                    y_pos.text = 0
                    z_pos.text = -1
                    root.module.position = Qt.vector3d(x_pos.text, y_pos.text, z_pos.text)
                }
            }
        }

        STPropertyPanel {
            id: rotPanel
            Layout.columnSpan: 2
            Layout.fillWidth: true
            title: "Parent-relative Rotation"
            collapsible: true

            property vector3d angles: root.module.orientation.toEulerAngles()

            function update_from_angles() {
                root.module.set_from_angles(
                    Qt.vector3d(x_euler.text, y_euler.text, z_euler.text))
            }

            STPropertyLabel { text: "X Angle" }
            STTextField {
                id: x_euler
                Layout.fillWidth: true
                text: rotPanel.angles.x
                validator: DoubleValidator {}
                onAccepted: rotPanel.update_from_angles()
            }

            STPropertyLabel { text: "Y Angle" }
            STTextField {
                id: y_euler
                Layout.fillWidth: true
                text: rotPanel.angles.y
                validator: DoubleValidator {}
                onAccepted: rotPanel.update_from_angles()
            }

            STPropertyLabel { text: "Z Angle" }
            STTextField {
                id: z_euler
                Layout.fillWidth: true
                text: rotPanel.angles.z
                validator: DoubleValidator {}
                onAccepted: rotPanel.update_from_angles()
            }

            STButton {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: "Reset"
                onClicked: {
                    x_euler.text = 0
                    y_euler.text = 0
                    z_euler.text = 0
                    rotPanel.update_from_angles()
                }
            }

            STButton {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "Point at..."
                onClicked: look_at_pop.open()

                STPopup {
                    id: look_at_pop

                    function accept_position() {
                        root.module.look_at_world_position(
                            Qt.vector3d(Number(look_at_x.text),
                                        Number(look_at_y.text),
                                        Number(look_at_z.text)))
                        close()
                    }

                    ColumnLayout {
                        spacing: 8

                        STComboBar {
                            id: look_at_mode
                            Layout.fillWidth: true
                            model: ["Position", "Entity"]
                            iconModel: ["\uf3c5", "\uf6d1"]
                        }

                        GridLayout {
                            visible: look_at_mode.currentIndex === 0
                            Layout.fillWidth: true
                            columns: 2

                            STPropertyLabel { text: "X" }
                            STTextField {
                                id: look_at_x
                                Layout.fillWidth: true
                                text: "0"
                                validator: DoubleValidator {}
                                onAccepted: look_at_pop.accept_position()
                            }

                            STPropertyLabel { text: "Y" }
                            STTextField {
                                id: look_at_y
                                Layout.fillWidth: true
                                text: "0"
                                validator: DoubleValidator {}
                                onAccepted: look_at_pop.accept_position()
                            }

                            STPropertyLabel { text: "Z" }
                            STTextField {
                                id: look_at_z
                                Layout.fillWidth: true
                                text: "0"
                                validator: DoubleValidator {}
                                onAccepted: look_at_pop.accept_position()
                            }
                        }

                        STButton {
                            visible: look_at_mode.currentIndex === 0
                            Layout.fillWidth: true
                            text: "Point at Position"
                            onClicked: look_at_pop.accept_position()
                        }

                        STButton {
                            visible: look_at_mode.currentIndex === 1
                            Layout.fillWidth: true
                            text: "Choose Entity"
                            onClicked: look_at_entity_pop.open()

                            SelectEntityPopup {
                                id: look_at_entity_pop
                                exclude: [root.module.entity]
                                onSelectedEntity: (entity) => {
                                    root.module.look_at_entity(entity)
                                    look_at_pop.close()
                                }
                            }
                        }
                    }
                }
            }
        }

        STSwitch {
            Layout.columnSpan: 2
            text: "Hidden"
            checked: root.module.hidden
            onToggled: root.module.hidden = checked
        }

        STSwitch {
            Layout.columnSpan: 2
            text: "Disabled"
            checked: root.module.disabled
            onToggled: root.module.disabled = checked
        }
    }
}
