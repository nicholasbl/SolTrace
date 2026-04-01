import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Dialogs
import SolTrace

Item {
    id: root
    property var left_panel_size: App.view.left_panel_size
    property var module : App.layout

    // Layout editor opens when App.view.editing_layout is true
    Connections {
        target: App.view
        function onEditing_layout_changed() {
            if (App.view.editing_layout && stack.depth === 1) {
                stack.push(entity_edit_view)
            } else if (!App.view.editing_layout && stack.depth > 1) {
                stack.pop()
            }
        }
    }

    StackView {
        id: stack
        initialItem: entity_list_view
        anchors.fill: parent
    }

    Component {
        id: entity_list_view

        ColumnLayout {
            STTextField {
                Layout.fillWidth: true

                leftIcon: "\uf002"

                placeholderText: "Filter..."
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    Layout.fillWidth: true
                    text: "With Material..."
                    flat: true
                }
                Button {
                    Layout.fillWidth: true
                    text: "With Geometry..."
                    flat: true
                }
            }

            ListView {
                Layout.fillHeight: true
                Layout.fillWidth: true

                clip: true


                model: root.module.root_elements_model

                ScrollIndicator.vertical: ScrollIndicator { }

                delegate: ItemDelegate {
                    required property string name
                    required property var entity
                    width: ListView.view.width

                    text: name

                    onClicked: {
                        root.module.current_element = entity
                        App.view.editing_layout = true
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
        id: entity_edit_view

        ColumnLayout {
            RowLayout {
                STIconButton {
                    text: "\uf053"
                    onClicked: App.view.editing_layout = false
                }
                Label {
                    text: "Element:"
                }
                Label {
                    text: module.current_database ?
                              module.current_database.name_of(module.current_element) :
                              "None"
                }
            }


            ScrollView {
                id: left_scroll
                Layout.fillHeight: true
                Layout.fillWidth: true

                contentWidth: availableWidth

                InstanceEdit {
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
