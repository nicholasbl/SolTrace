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
            id: positionPanel
            Layout.columnSpan: 2
            Layout.fillWidth: true
            title: "Position"
            collapsible: true

            STComboBar {
                id: positionModeBar
                Layout.columnSpan: 2
                Layout.fillWidth: true
                model: ["Local", "Global"]
            }

            StackLayout {
                id: positionSwipe
                Layout.columnSpan: 2
                Layout.fillWidth: true
                //Layout.preferredHeight: currentItem ? currentItem.implicitHeight : 0
                currentIndex: positionModeBar.currentIndex

                GridLayout {
                    id: localPositionEditor
                    width: positionSwipe.width
                    columns: 2

                    function update_position_if_valid() {
                        if (!(local_x_pos.acceptableInput
                              && local_y_pos.acceptableInput
                              && local_z_pos.acceptableInput)) {
                            return
                        }

                        root.module.position = Qt.vector3d(Number(local_x_pos.text),
                                                           Number(local_y_pos.text),
                                                           Number(local_z_pos.text))
                    }

                    STPropertyLabel { text: "X" }
                    STTextField {
                        id: local_x_pos
                        Layout.fillWidth: true
                        text: root.module.position.x
                        validator: DoubleValidator {}
                        onAccepted: localPositionEditor.update_position_if_valid()
                        onTextEdited: localPositionEditor.update_position_if_valid()
                    }

                    STPropertyLabel { text: "Y" }
                    STTextField {
                        id: local_y_pos
                        Layout.fillWidth: true
                        text: root.module.position.y
                        validator: DoubleValidator {}
                        onAccepted: localPositionEditor.update_position_if_valid()
                        onTextEdited: localPositionEditor.update_position_if_valid()
                    }

                    STPropertyLabel { text: "Z" }
                    STTextField {
                        id: local_z_pos
                        Layout.fillWidth: true
                        text: root.module.position.z
                        validator: DoubleValidator {}
                        onAccepted: localPositionEditor.update_position_if_valid()
                        onTextEdited: localPositionEditor.update_position_if_valid()
                    }

                    STButton {
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        text: "Set Local Default Position"
                        onClicked: {
                            local_x_pos.text = 0
                            local_y_pos.text = 0
                            local_z_pos.text = -1
                            localPositionEditor.update_position_if_valid()
                        }
                    }
                }

                GridLayout {
                    id: globalPositionEditor
                    width: positionSwipe.width
                    columns: 2

                    function update_position_if_valid() {
                        if (!(global_x_pos.acceptableInput
                              && global_y_pos.acceptableInput
                              && global_z_pos.acceptableInput)) {
                            return
                        }

                        root.module.global_position =
                                Qt.vector3d(Number(global_x_pos.text),
                                            Number(global_y_pos.text),
                                            Number(global_z_pos.text))
                    }

                    STPropertyLabel { text: "X" }
                    STTextField {
                        id: global_x_pos
                        Layout.fillWidth: true
                        text: root.module.global_position.x
                        validator: DoubleValidator {}
                        onAccepted: globalPositionEditor.update_position_if_valid()
                        onTextEdited: globalPositionEditor.update_position_if_valid()
                    }

                    STPropertyLabel { text: "Y" }
                    STTextField {
                        id: global_y_pos
                        Layout.fillWidth: true
                        text: root.module.global_position.y
                        validator: DoubleValidator {}
                        onAccepted: globalPositionEditor.update_position_if_valid()
                        onTextEdited: globalPositionEditor.update_position_if_valid()
                    }

                    STPropertyLabel { text: "Z" }
                    STTextField {
                        id: global_z_pos
                        Layout.fillWidth: true
                        text: root.module.global_position.z
                        validator: DoubleValidator {}
                        onAccepted: globalPositionEditor.update_position_if_valid()
                        onTextEdited: globalPositionEditor.update_position_if_valid()
                    }

                    STButton {
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        text: "Set Global Default Position"
                        onClicked: {
                            global_x_pos.text = 0
                            global_y_pos.text = 0
                            global_z_pos.text = -1
                            globalPositionEditor.update_position_if_valid()
                        }
                    }
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
                if (!(x_euler.acceptableInput && y_euler.acceptableInput && z_euler.acceptableInput)) {
                    return
                }

                root.module.set_from_angles(
                            Qt.vector3d(Number(x_euler.text),
                                        Number(y_euler.text),
                                        Number(z_euler.text)))
            }

            STPropertyLabel { text: "X Angle (deg)" }
            STTextField {
                id: x_euler
                Layout.fillWidth: true
                text: rotPanel.angles.x
                validator: DoubleValidator {}
                onAccepted: rotPanel.update_from_angles()
                onTextEdited: rotPanel.update_from_angles()
            }

            STPropertyLabel { text: "Y Angle (deg)" }
            STTextField {
                id: y_euler
                Layout.fillWidth: true
                text: rotPanel.angles.y
                validator: DoubleValidator {}
                onAccepted: rotPanel.update_from_angles()
                onTextEdited: rotPanel.update_from_angles()
            }

            STPropertyLabel { text: "Z Angle (deg)" }
            STTextField {
                id: z_euler
                Layout.fillWidth: true
                text: rotPanel.angles.z
                validator: DoubleValidator {}
                onAccepted: rotPanel.update_from_angles()
                onTextEdited: rotPanel.update_from_angles()
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
