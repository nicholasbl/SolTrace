import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root

    property var module: App.layout.instance_edit
    property bool singleColumn: App.view.left_panel.size === SplitPanelData.Small
    property var labelAlignment: (singleColumn ? Qt.AlignLeft : Qt.AlignRight) | Qt.AlignVCenter

    STPropertyPanel {
        Layout.fillWidth: true
        columns: 2

        Label {
            Layout.columnSpan: 2
            Layout.fillWidth: true

            visible: root.module.current_material_name.length === 0
                     || root.module.current_geometry_name.length === 0
            text: "A material and geometry must be assigned for the element to be visible."
            color: Material.color(Material.Yellow)
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        }

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
            SelectElementPopup {
                id: parent_pop
                exclude: [root.module.entity]
                allowNothing: true
                onSelectedElement: (element) => root.module.parent = element
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

        STPropertySeparator {
            title: "Placement"
        }

        STSwitch {
            Layout.columnSpan: 2
            text: "Use 3D widget"
            checked: App.view.mouse_mode === ViewModule.EditElement
            onToggled: App.view.mouse_mode = checked ? ViewModule.EditElement
                                                      : ViewModule.Camera
        }

        InlineDocumentation {
            key: "configure.layout.coordinates"
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        STPropertyLabel {
            text: "Coordinates"
            Layout.alignment: root.labelAlignment
            Layout.columnSpan: root.singleColumn ? 2 : 1
        }

        STComboBox {
            id: positionModeCombo
            Layout.columnSpan: root.singleColumn ? 2 : 1
            Layout.fillWidth: true
            model: ["Local", "Global"]
        }

        StackLayout {
            id: positionSwipe
            Layout.columnSpan: 2
            Layout.fillWidth: true
            //Layout.preferredHeight: currentItem ? currentItem.implicitHeight : 0
            currentIndex: positionModeCombo.currentIndex

            GridLayout {
                id: localPositionEditor
                Layout.preferredWidth: positionSwipe.width
                columns: 2

                function update_position() {
                    root.module.position = Qt.vector3d(local_x_pos.value,
                                                       local_y_pos.value,
                                                       local_z_pos.value)
                }

                STPropertyLabel { text: "X" }
                STDoubleSpinBox {
                    id: local_x_pos
                    Layout.fillWidth: true
                    value: root.module.position.x
                    onValueModified: localPositionEditor.update_position()
                    decimals: 4
                }

                STPropertyLabel { text: "Y" }
                STDoubleSpinBox {
                    id: local_y_pos
                    Layout.fillWidth: true
                    value: root.module.position.y
                    onValueModified: localPositionEditor.update_position()
                    decimals: 4
                }

                STPropertyLabel { text: "Z" }
                STDoubleSpinBox {
                    id: local_z_pos
                    Layout.fillWidth: true
                    value: root.module.position.z
                    onValueModified: localPositionEditor.update_position()
                    decimals: 4
                }

                STButton {
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    text: "Reset Local Default Position"
                    onClicked: {
                        root.module.position = Qt.vector3d(0, 0, -1)
                    }
                }
            }

            GridLayout {
                id: globalPositionEditor
                Layout.preferredWidth: positionSwipe.width
                columns: 2

                function update_position() {
                    root.module.global_position =
                            Qt.vector3d(global_x_pos.value,
                                        global_y_pos.value,
                                        global_z_pos.value)
                }

                STPropertyLabel { text: "X" }
                STDoubleSpinBox {
                    id: global_x_pos
                    Layout.fillWidth: true
                    value: root.module.global_position.x
                    onValueModified: globalPositionEditor.update_position()
                    decimals: 4
                }

                STPropertyLabel { text: "Y" }
                STDoubleSpinBox {
                    id: global_y_pos
                    Layout.fillWidth: true
                    value: root.module.global_position.y
                    onValueModified: globalPositionEditor.update_position()
                    decimals: 4
                }

                STPropertyLabel { text: "Z" }
                STDoubleSpinBox {
                    id: global_z_pos
                    Layout.fillWidth: true
                    value: root.module.global_position.z
                    onValueModified: globalPositionEditor.update_position()
                    decimals: 4
                }

                STButton {
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    text: "Reset Global Default Position"
                    onClicked: {
                        root.module.global_position = Qt.vector3d(0, 0, -1)
                    }
                }
            }
        }

        STPropertySeparator {
            id: rotPanel
            title: "Parent-relative Rotation"

            property vector3d angles: root.module.orientation.toEulerAngles()

            function update_from_angles() {
                root.module.set_from_angles(
                            Qt.vector3d(x_euler.value,
                                        y_euler.value,
                                        z_euler.value))
            }
        }

            STPropertyLabel { text: "X Angle (deg)" }
            STDoubleSpinBox {
                id: x_euler
                Layout.fillWidth: true
                value: rotPanel.angles.x
                onValueModified: rotPanel.update_from_angles()
                from: -180
                to: 180
            }

            STPropertyLabel { text: "Y Angle (deg)" }
            STDoubleSpinBox {
                id: y_euler
                Layout.fillWidth: true
                value: rotPanel.angles.y
                onValueModified: rotPanel.update_from_angles()
                from: -90
                to: 90
            }

            STPropertyLabel { text: "Z Angle (deg)" }
            STDoubleSpinBox {
                id: z_euler
                Layout.fillWidth: true
                value: rotPanel.angles.z
                onValueModified: rotPanel.update_from_angles()
                from: -180
                to: 180
            }

            STButton {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: "Reset"
                onClicked: {
                    root.module.set_from_angles(Qt.vector3d(0, 0, 0))
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
                                    Qt.vector3d(look_at_x.value,
                                                look_at_y.value,
                                                look_at_z.value))
                    }

                    ColumnLayout {
                        spacing: 8

                        STComboBar {
                            id: look_at_mode
                            Layout.fillWidth: true
                            model: ["Position", "Element"]
                            iconModel: ["\uf3c5", "\uf6d1"]
                        }

                        GridLayout {
                            visible: look_at_mode.currentIndex === 0
                            Layout.fillWidth: true
                            columns: 2

                            STPropertyLabel { text: "X" }
                            STDoubleSpinBox {
                                id: look_at_x
                                Layout.fillWidth: true
                                value: 0
                                decimals: 4
                                onValueModified: look_at_pop.accept_position()
                            }

                            STPropertyLabel { text: "Y" }
                            STDoubleSpinBox {
                                id: look_at_y
                                Layout.fillWidth: true
                                value: 0
                                decimals: 4
                                onValueModified: look_at_pop.accept_position()
                            }

                            STPropertyLabel { text: "Z" }
                            STDoubleSpinBox {
                                id: look_at_z
                                Layout.fillWidth: true
                                value: 0
                                decimals: 4
                                onValueModified: look_at_pop.accept_position()
                            }
                        }

                        STButton {
                            visible: look_at_mode.currentIndex === 0
                            Layout.fillWidth: true
                            text: "Point at Position"
                            onClicked: {
                                look_at_pop.accept_position()
                                look_at_pop.close()
                            }
                        }

                        STButton {
                            visible: look_at_mode.currentIndex === 1
                            Layout.fillWidth: true
                            text: "Choose Element"
                            onClicked: look_at_element_pop.open()

                            SelectElementPopup {
                                id: look_at_element_pop
                                exclude: [root.module.entity]
                                onSelectedElement: (element) => {
                                                      root.module.look_at_entity(element)
                                                      look_at_pop.close()
                                                  }
                            }
                        }
                    }
                }
            }

        STPropertySeparator {
            title: "Visualization"
        }

        STSwitch {
            Layout.columnSpan: 2
            text: "Hidden"
            checked: root.module.hidden
            onToggled: root.module.hidden = checked
        }

        ColorPickerField {
            id: elementColorPicker
            Layout.columnSpan: 2
            Layout.preferredWidth: 200
            color: root.module.color
            label: "Element Color"
            onUpdated: {
                root.module.color = elementColorPicker.color
            }
        }

        STSwitch {
            Layout.columnSpan: 2
            text: "Disabled"
            checked: root.module.disabled
            onToggled: root.module.disabled = checked
        }

        STSwitch {
            Layout.columnSpan: 2
            text: "Virtual"
            checked: root.module.virtual_element
            onToggled: root.module.virtual_element = checked
        }
    }
}
