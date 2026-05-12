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

    model: module.filtered_root_elements_model
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

        RowLayout {
            Layout.fillWidth: true

            STSearchField {
                id: search_field
                Layout.fillWidth: true

                text: root.model.name_filter

                Binding {
                    target: root.model
                    property: "name_filter"
                    value: search_field.text
                }
            }

            STIconButton {
                text: "\uf0b0"

                onClicked: filter_popup.open()

                STPopup {
                    id: filter_popup

                    GridLayout {
                        columns: 3

                        Label {
                            Layout.fillWidth: true
                            Layout.columnSpan: 3

                            text: "Filter entities by:"
                        }

                        Label {
                            text: "Material:"
                        }

                        STClickableLabel {
                            Layout.fillWidth: true
                            property string mat_name: root.model.material_filter_name
                            text: mat_name.length ? mat_name : "All"

                            onClicked: mat_pop.open()

                            SelectItemPopup {
                                id: mat_pop
                                source_model: AppData.materials.materials_list

                                onSelectedEntity: (entity) =>
                                                  root.model.material_filter = entity
                            }
                        }

                        STIconButton {
                            text: "\uf0e2"
                            onClicked: root.model.clear_material()
                        }

                        Label {
                            text: "Geometry:"
                        }

                        STClickableLabel {
                            Layout.fillWidth: true
                            property string geo_name: root.model.geometry_filter_name
                            text: geo_name.length ? geo_name : "All"

                            onClicked: geo_pop.open()

                            SelectItemPopup {
                                id: geo_pop
                                source_model: AppData.materials.geometry_list

                                onSelectedEntity: (entity) =>
                                                  root.model.geometry_filter = entity
                            }
                        }

                        STIconButton {
                            text: "\uf0e2"
                            onClicked: root.model.clear_geometry()
                        }

                        STButton {
                            text: "Reset All"

                            onClicked: root.model.clear_all_filters()
                        }

                    }
                }
            }

            STIconButton {
                id: clear_filter_button

                visible: root.model.has_filter

                text: "\ue17b"

                onClicked: root.model.clear_all_filters()
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

            RenameLabel {
                text: root.module.current_element_name

                font.family: "CMU Serif"
                font.pointSize: 16
                font.bold: true

                onAccepted: (new_name) => {
                    var curr_db = root.module.current_database

                    new_name = curr_db.sanitize_entity_name(new_name);

                    curr_db.set_name_of(
                                    root.module.current_element,
                                    new_name
                                    )
                }
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
