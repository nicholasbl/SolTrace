import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

Popup {
    id: root
    parent: Overlay.overlay

    property var module: AppData.script

    height: parent.height * .9
    width: parent.width * .9

    modal: true

    opened: text_edit_area.text = root.module.code

    ColumnLayout {
        TextArea {
            id: text_edit_area
            Layout.fillWidth: true
            Layout.fillHeight: true

            onTextEdited: {
                root.module.code = text
            }
        }

        Label {
            id: status_label

            property bool has_parse_error: root.module.parse_errors.length

            text: has_parse_error ? "Parse error" : "Ok"
        }

        Label {
            Layout.fillWidth: true
            Layout.preferredHeight: 240

            text: (root.module.parse_errors.concat(root.module.run_errors)).join("\n")
        }
    }
}
