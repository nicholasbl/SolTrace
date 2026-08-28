import QtQuick
import QtQuick.Layouts

import SolTrace

GridLayout {
    id: root

    property var module
    property bool singleColumn: false
    property int labelAlignment: Qt.AlignRight | Qt.AlignVCenter
    property int childColumnSpan: singleColumn ? 1 : 2

    property real xAngle: 0
    property real yAngle: 0
    property real zAngle: 0

    columns: root.singleColumn ? 1 : 2

    function set_angle_values(x, y, z) {
        xAngle = x
        yAngle = y
        zAngle = z
        x_euler.value = x
        y_euler.value = y
        z_euler.value = z
    }

    function refresh_angles() {
        const angles = root.module.euler_angles_xyz
        set_angle_values(angles.x, angles.y, angles.z)
    }

    function update_from_angles() {
        root.module.euler_angles_xyz = Qt.vector3d(xAngle, yAngle, zAngle)
    }

    Component.onCompleted: refresh_angles()

    Connections {
        target: root.module

        function onEuler_angles_xyz_changed() {
            root.refresh_angles()
        }
    }

    STPropertySeparator {
        title: "Parent-relative Rotation"
    }

    STPropertyLabel {
        text: "X Angle (deg)"
        Layout.alignment: root.labelAlignment
    }
    STDoubleSpinBox {
        id: x_euler
        Layout.fillWidth: true
        from: -Infinity
        to: Infinity
        onValueModified: {
            root.xAngle = value
            root.update_from_angles()
        }
    }

    STPropertyLabel {
        text: "Y Angle (deg)"
        Layout.alignment: root.labelAlignment
    }
    STDoubleSpinBox {
        id: y_euler
        Layout.fillWidth: true
        from: -Infinity
        to: Infinity
        onValueModified: {
            root.yAngle = value
            root.update_from_angles()
        }
    }

    STPropertyLabel {
        text: "Z Angle (deg)"
        Layout.alignment: root.labelAlignment
    }
    STDoubleSpinBox {
        id: z_euler
        Layout.fillWidth: true
        from: -Infinity
        to: Infinity
        onValueModified: {
            root.zAngle = value
            root.update_from_angles()
        }
    }

    STButton {
        Layout.fillWidth: true
        Layout.columnSpan: root.childColumnSpan
        text: "Reset"
        onClicked: root.module.euler_angles_xyz = Qt.vector3d(0, 0, 0)
    }

    STButton {
        Layout.columnSpan: root.childColumnSpan
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
                        from: -Infinity
                        to: Infinity
                        decimals: 4
                        onValueModified: look_at_pop.accept_position()
                    }

                    STPropertyLabel { text: "Y" }
                    STDoubleSpinBox {
                        id: look_at_y
                        Layout.fillWidth: true
                        value: 0
                        from: -Infinity
                        to: Infinity
                        decimals: 4
                        onValueModified: look_at_pop.accept_position()
                    }

                    STPropertyLabel { text: "Z" }
                    STDoubleSpinBox {
                        id: look_at_z
                        Layout.fillWidth: true
                        value: 0
                        from: -Infinity
                        to: Infinity
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
}
