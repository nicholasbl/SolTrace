import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

AdaptiveEditor {
    id: root

    model: App.materials.geometry_list
    wideThreshold: 500
    listWidth: 250

    onEditingChanged: {
        App.view.editing_geometry = editing
    }

    Connections {
        target: App.view
        function onEditing_geometry_changed() {
            root.editing = App.view.editing_geometry
        }
    }

    onItemClicked: function(index, modelData) {
        // if (App.db) App.db.clear_selection()
        App.materials.current_geometry = modelData.entity
        // if (App.db) App.db.select_all_with_geometry(modelData.entity)
    }

    listHeader: RowLayout {
        STTextField {
            Layout.fillWidth: true
            leftIcon: "\uf002"
            placeholderText: "Search..."
        }
    }

    listFooter: RowLayout {
        CreateNewItemButton {
            title: "New Geometry"

            onCreateRequested: function(name) {
                var new_name = AppData.current_database.sanitize_geometry_name(name)
                AppData.current_database.add_geometry_group(new_name)
            }
        }
    }

    listDelegate: STItemDelegate {
        text: itemModel ? itemModel.name : "No name"
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
                text: root.App.materials.current_geometry_name

                font.family: "CMU Serif"
                font.pointSize: 16
                font.bold: true
            }
        }

        SurfacePreviewScene {
            Layout.fillWidth: true
            Layout.preferredHeight: 148
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            contentWidth: availableWidth

            GeometryProperties {
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
            text: "Select a geometry"
            font.pointSize: 16
            opacity: 0.5
        }
    }
}
