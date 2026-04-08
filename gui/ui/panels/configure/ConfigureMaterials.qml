import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

AdaptiveEditor {
    id: root

    model: App.materials.materials_list
    wideThreshold: 500
    listWidth: 250

    onEditingChanged: {
        App.view.editing_material = editing
    }

    Connections {
        target: App.view
        function onEditing_material_changed() {
            root.editing = App.view.editing_material
        }
    }

    onItemClicked: function(index, modelData) {
        App.materials.current_material = modelData.entity
    }

    listHeader: RowLayout {
        STTextField {
            Layout.fillWidth: true
            leftIcon: "\uf002"
            placeholderText: "Search..."
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
        spacing: 8

        RowLayout {
            STIconButton {
                text: "\uf053"
                visible: !root.wideMode
                onClicked: root.goBack()
            }

            Label {
                text: App.materials.current_material_name

                font.family: "CMU Serif"
                font.pointSize: 16
                font.bold: true
            }
        }

        SurfacePropertyGraphic {
            Layout.fillWidth: true
            Layout.preferredHeight: 148

            property var face: bar.currentIndex == 0
                ? App.materials.material_edit.front_editor
                : App.materials.material_edit.back_editor

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
            model: ["Front", "Back"]
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            contentWidth: availableWidth

            SwipeView {
                anchors.fill: parent
                interactive: false
                clip: true
                currentIndex: bar.currentIndex

                MaterialOpticals {
                    collapsed: false
                    Layout.fillWidth: true
                    side_editor: App.materials.material_edit.front_editor
                }

                MaterialOpticals {
                    collapsed: false
                    Layout.fillWidth: true
                    side_editor: App.materials.material_edit.back_editor
                }
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
            text: "Select a material"
            font.pointSize: 16
            opacity: 0.5
        }
    }
}
