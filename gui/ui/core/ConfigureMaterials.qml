import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

Item {
    id: root
    property var materials_module : App.materials

    // Material editor opens when App.view.editing_material is true
    Connections {
        target: App.view
        function onEditing_material_changed() {
            if (App.view.editing_material && stack.depth === 1) {
                stack.push(material_edit_view)
            } else if (!App.view.editing_material && stack.depth > 1) {
                stack.pop()
            }
        }
    }

    StackView {
        id: stack
        initialItem: material_list_view
        anchors.fill: parent
    }

    Component {
        id: material_list_view

        ColumnLayout {
            STTextField {
                Layout.fillWidth: true

                leftIcon: "\uf002"

                placeholderText: "Search..."
            }

            ListView {
                Layout.fillHeight: true
                Layout.fillWidth: true

                clip: true


                model: materials_module.materials_list

                ScrollIndicator.vertical: ScrollIndicator { }

                delegate: ItemDelegate {
                    required property string name
                    required property var entity
                    width: ListView.view.width

                    text: name

                    onClicked: {
                        materials_module.current_material = entity
                        App.view.editing_material = true
                    }

                    background: Rectangle {
                        implicitHeight: 24
                        implicitWidth: 100
                        opacity: enabled ? 1 : 0.3
                        color: parent.down
                               ? Material.rippleColor : "transparent"
                    }
                }
            }
            RowLayout {
                STIconButton {
                    text: "\uf055"
                }
            }
        }
    }

    Component {
        id: material_edit_view

        ColumnLayout {
            RowLayout {
                STIconButton {
                    text: "\uf053"
                    onClicked: {
                        App.view.editing_material = false
                    }
                }

                Label {
                    text: materials_module.current_material_name
                }
            }

            SurfacePropertyGraphic {
                Layout.fillWidth: true

                Layout.preferredHeight: 148

                property var face : bar.currentIndex == 0 ?
                                        materials_module.material_edit.front_editor
                                      :
                                        materials_module.material_edit.back_editor


                reflectance: face.reflectivity
                transmittance: face.transmitivity
                nFront: face.refraction_index_front
                nBack: face.refraction_index_back
                slopeErrorMrad: face.slope_error
                specularityErrorMrad: face.specularity_error
            }

            STComboBar {
                id: bar

                Layout.fillWidth: true

                model : [
                    "Front",
                    "Back",
                ]

            }


            ScrollView {
                id: left_scroll
                Layout.fillHeight: true
                Layout.fillWidth: true

                contentWidth: availableWidth

                SwipeView {
                    id: edit_view
                    anchors.fill: parent

                    interactive: false

                    clip: true

                    currentIndex: bar.currentIndex

                    MaterialOpticals {
                        collapsed: false
                        //title: "Front Side"
                        Layout.fillWidth: true
                        side_editor: materials_module.material_edit.front_editor
                    }

                    MaterialOpticals {
                        collapsed: false
                        //title: "Back Side"
                        Layout.fillWidth: true
                        side_editor: materials_module.material_edit.back_editor
                    }

                    // ColumnLayout {
                    //     spacing: 12

                    //     GeometryPropPanel {
                    //         Layout.fillWidth: true
                    //     }

                    //     AperturePropPanel {
                    //         Layout.fillWidth: true
                    //     }
                    // }
                }
            }

            RowLayout {

                STIconButton {
                    text: "\uf2ed"
                }
            }
        }
    }
}
