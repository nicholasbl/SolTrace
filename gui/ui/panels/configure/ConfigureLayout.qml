import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Dialogs
import SolTrace

AdaptiveEditor {
    id: root

    property var module: App.layout

    model: module.root_elements_model
    wideThreshold: 500
    listWidth: 250

    onEditingChanged: {
        App.view.editing_layout = editing
    }

    Connections {
        target: App.view
        function onEditing_layout_changed() {
            root.editing = App.view.editing_layout
        }
    }

    onItemClicked: function(index, modelData) {
        module.current_element = modelData.entity
    }

    listHeader: ColumnLayout {
        spacing: 0

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
    }

    listFooter: RowLayout {
        STIconButton {
            text: "\uf055"
        }
    }

    listDelegate: STItemDelegate {
        text: itemModel ? itemModel.name : "Unnamed"
        highlighted: isCurrent
    }

    detailView: ColumnLayout {

        RowLayout {
            STIconButton {
                text: "\uf053"
                visible: !root.wideMode
                onClicked: root.goBack()
            }

            Label {
                text: "Element:"

                font.family: "CMU Serif"
                font.pointSize: 16
                font.bold: true
            }

            Label {
                text: root.module.current_database
                      ? root.module.current_database.name_of(module.current_element)
                      : "None"

                font.family: "CMU Serif"
                font.pointSize: 16
                font.bold: true
            }
        }

        ScrollView {
            id: layout_scroll
            Layout.fillHeight: true
            Layout.fillWidth: true
            contentWidth: availableWidth

            InstanceEdit {
                width: layout_scroll.availableWidth
                height: implicitHeight
            }
        }

        RowLayout {
            STIconButton {
                text: "\uf2ed"
            }
        }
    }

    placeholder: Item {
        Label {
            anchors.centerIn: parent
            text: "Select an element"
            font.pointSize: 16
            opacity: 0.5
        }
    }
}
