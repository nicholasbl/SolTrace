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

    function load_script(url) {
        const request = new XMLHttpRequest()
        request.open("GET", url)
        request.onreadystatechange = function() {
            if (request.readyState === XMLHttpRequest.DONE) {
                if (request.status === 0 || request.status === 200) {
                    root.module.code = request.responseText
                } else {
                    console.warn("Unable to load script", url, request.status)
                }
            }
        }
        request.send()
    }

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

                    onClicked: root.load_script("qrc:/assets/scripts/simple.js")
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

                Layout.row: 0

                text: root.module.title
                font.bold: true
            }

            Label {
                visible: root.module.description.length
                Layout.fillWidth: true
                Layout.columnSpan: 2

                Layout.row: 1

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
                    Layout.row: index + 2
                    Layout.fillWidth: true
                    text: name
                }
            }

            Repeater {
                model: root.module.properties

                delegate: STTextField {
                    required property string value
                    required property int index

                    Layout.column: 1
                    Layout.row: index + 2
                    Layout.fillWidth: true

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
