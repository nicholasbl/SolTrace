import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ScrollView {
    Layout.fillHeight: true
    Layout.fillWidth: true

    id: root

    property var module: AppData.script

    contentWidth: availableWidth

    ColumnLayout {
        width: root.availableWidth

        STPropertyPanel {
            Layout.fillWidth: true

            title: "Source"
            collapsible: true

            RowLayout {
                uniformCellSizes: true

                Layout.fillWidth: true
                Layout.columnSpan: 2

                STButton {
                    Layout.fillWidth: true

                    text: "New"
                }

                STButton {
                    Layout.fillWidth: true

                    text: "Load"
                }

                STButton {
                    Layout.fillWidth: true

                    text: "Edit"

                    onClicked: script_editor.open()

                    ScriptEditor {
                        id: script_editor
                    }
                }
            }
        }

        STPropertyPanel {
            Layout.fillWidth: true

            title: "Script"
            collapsible: true

            Label {
                visible: root.module.title.length
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: root.module.title
                font.bold: true
            }

            Label {
                visible: root.module.description.length
                Layout.fillWidth: true
                Layout.columnSpan: 2
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                text: root.module.description
            }

            Repeater {
                id: label_repeater
                model: root.module.properties

                delegate: STPropertyLabel {
                    required property string name
                    required property int index

                    Layout.column: 0
                    Layout.row: index
                    text: name
                }
            }

            Repeater {
                model: root.module.properties

                delegate: STTextField {
                    required property string value
                    required property int index

                    Layout.column: 1
                    Layout.row: index

                    text: value

                    onAccepted: value = text
                }
            }

            STButton {
                text: "Run"
                Layout.columnSpan: 2
                Layout.fillWidth: true

                enabled: root.module.valid

                onClicked: root.module.run()
            }


        }

    }
}
