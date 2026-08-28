import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

STPropertyPanel {
    id: root

    property var module
    property bool singleColumn: false
    property int labelAlignment: Qt.AlignRight | Qt.AlignVCenter
    property int childColumnSpan: singleColumn ? 1 : 2

    columns: singleColumn ? 1 : 2

    Label {
        Layout.columnSpan: root.childColumnSpan
        Layout.fillWidth: true

        visible: root.module.current_material_name.length === 0
                 || root.module.current_geometry_name.length === 0
        text: "A material and geometry must be assigned for the element to be visible."
        color: Material.color(Material.Yellow)
        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
    }

    STPropertyLabel {
        text: "Parent"
        Layout.alignment: root.labelAlignment
        Layout.columnSpan: 1
    }

    STButton {
        Layout.fillWidth: true
        Layout.columnSpan: 1
        property string parent_name: root.module.parent_name
        text: parent_name.length ? parent_name : "Unassigned"
        onClicked: parent_pop.open()
        SelectElementPopup {
            id: parent_pop
            exclude: [root.module.entity]
            allowNothing: true
            onSelectedElement: (element) => root.module.parent = element
            onSelectedNothing: root.module.clear_parent()
        }
    }

    STPropertyLabel {
        text: "Material"
        Layout.alignment: root.labelAlignment
        Layout.columnSpan: 1
    }

    STButton {
        Layout.fillWidth: true
        Layout.columnSpan: 1
        property string material_name: root.module.current_material_name
        text: material_name.length ? material_name : "Unassigned"
        onClicked: material_pop.open()
        SelectItemPopup {
            id: material_pop
            source_model: AppData.materials.materials_list
            onSelectedEntity: (entity) => root.module.current_material = entity
        }
    }

    STPropertyLabel {
        text: "Geometry"
        Layout.alignment: root.labelAlignment
        Layout.columnSpan: 1
    }

    STButton {
        Layout.fillWidth: true
        Layout.columnSpan: 1
        property string geometry_name: root.module.current_geometry_name
        text: geometry_name.length ? geometry_name : "Unassigned"
        onClicked: geometry_pop.open()
        SelectItemPopup {
            id: geometry_pop
            source_model: AppData.materials.geometry_list
            onSelectedEntity: (entity) => root.module.current_geometry = entity
        }
    }
}
