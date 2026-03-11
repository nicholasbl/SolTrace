import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

Item {
    id: material_edit_ui
    readonly property bool hasSelection: App.materials.current_material_name.length > 0
    readonly property var groupEditor: App.materials.group_edit
    readonly property var frontEditor: groupEditor ? groupEditor.front_editor : null
    readonly property var backEditor: groupEditor ? groupEditor.back_editor : null

    ColumnLayout {
        anchors.fill: parent

        GlassTabBar {
            id: bar
            Layout.fillWidth: true
            model: ["Front Optics", "Back Optics", "Geometry"]

            onIndexRequested: function(index){
                edit_view.currentIndex = index
                bar.index = index
            }
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

                MaterialOpticals {
                    collapsed: false
                    //title: "Front Side"
                    Layout.fillWidth: true
                    enabled: material_edit_ui.hasSelection
                    side_editor: material_edit_ui.frontEditor
                }

                MaterialOpticals {
                    collapsed: false
                    //title: "Back Side"
                    Layout.fillWidth: true
                    enabled: material_edit_ui.hasSelection
                    side_editor: material_edit_ui.backEditor
                }

                ColumnLayout {
                    spacing: 12

                    GeometryPropPanel {
                        Layout.fillWidth: true
                    }

                    AperturePropPanel {
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}


