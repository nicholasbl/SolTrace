import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

Item {
    id: left_side

    ScrollView {
        id: left_scroll
        anchors.fill: parent

        contentWidth: availableWidth

        ColumnLayout {
            id: material_edit_ui

            width: left_scroll.availableWidth

            spacing: 16

            readonly property bool hasSelection: App.materials.current_material_name.length > 0
            readonly property var groupEditor: App.materials.group_edit
            readonly property var frontEditor: groupEditor ? groupEditor.front_editor : null
            readonly property var backEditor: groupEditor ? groupEditor.back_editor : null

            Rectangle {
                Layout.fillWidth: true
                radius: 12
                color: Qt.rgba(1, 1, 1, 0.04)
                border.width: 1
                border.color: Theme.lineColor
                implicitHeight: summary_copy.implicitHeight + 28

                Body {
                    id: summary_copy
                    anchors.fill: parent
                    anchors.margins: 14
                    text: material_edit_ui.hasSelection
                          ? "Edit front and back optical properties for the selected render group. Changes here update the live material preview on the right."
                          : "Pick a material from the left sidebar to begin editing its optical properties."
                }
            }

            MaterialOpticals {
                collapsed: true
                title: "Front Side"
                Layout.fillWidth: true
                enabled: material_edit_ui.hasSelection
                side_editor: material_edit_ui.frontEditor
            }

            MaterialOpticals {
                collapsed: true
                title: "Back Side"
                Layout.fillWidth: true
                enabled: material_edit_ui.hasSelection
                side_editor: material_edit_ui.backEditor
            }
        }
    }
}
