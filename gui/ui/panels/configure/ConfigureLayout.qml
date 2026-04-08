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

    listDelegate: ItemDelegate {
        text: itemModel.name
        highlighted: isCurrent
        width: parent ? parent.width : implicitWidth

        background: Rectangle {
            implicitHeight: 24
            implicitWidth: 100
            opacity: enabled ? 1 : 0.3
            color: parent.down ? Material.rippleColor : "transparent"
        }
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
            Layout.fillHeight: true
            Layout.fillWidth: true
            contentWidth: availableWidth

            InstanceEdit {
                anchors.fill: parent
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
