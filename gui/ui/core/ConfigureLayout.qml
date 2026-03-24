import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

Item {
    id: root
    property int size_class

    property var module : App.layout

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
                        stack.push(entity_edit_view)
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
                    onClicked: stack.pop()
                }
                Label {
                    text: "Element:"
                }
                Label {
                    text: module.database ?
                              module.database.name_of(module.current_element) :
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
