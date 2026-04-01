import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

Item {
    id: root
    property var left_panel_size: App.view.left_panel_size
    property var materials_module : App.materials

    // Geometry editor opens when App.view.editing_geometry is true
    Connections {
        target: App.view
        function onEditing_geometry_changed() {
            if (App.view.editing_geometry && stack.depth === 1) {
                stack.push(geometry_edit_view)
            } else if (!App.view.editing_geometry && stack.depth > 1) {
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


                model: materials_module.geometry_list

                ScrollIndicator.vertical: ScrollIndicator { }

                delegate: ItemDelegate {
                    required property string name
                    required property var entity
                    width: ListView.view.width

                    text: name

                    onClicked: {
                        materials_module.current_geometry = entity
                        App.view.editing_geometry = true
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
        id: geometry_edit_view

        ColumnLayout {
            RowLayout {
                STIconButton {
                    text: "\uf053"
                    onClicked: App.view.editing_geometry = false
                }

                Label {
                    text: materials_module.current_geometry_name
                }
            }

            SurfacePreviewScene {
                Layout.fillWidth: true

                Layout.preferredHeight: 148
            }


            ScrollView {
                id: left_scroll
                Layout.fillHeight: true
                Layout.fillWidth: true

                contentWidth: availableWidth

                GeometryProperties {
                    anchors.fill: parent
                    //width: parent.availableWidth
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
