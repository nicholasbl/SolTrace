import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root
    required property string key

    property PanelData target
    property string titlePrefix: ""
    property string title: ""
    property string body: ""

    visible: root.target ? root.target.inline_docs && root.target.size > 0 : true

    Text {
        id: title
        text: {
            App.docs.locale
            App.docs.version
            return root.title ? root.title : App.docs.get(root.key, "title")
        }

        Layout.fillWidth: true

        font.pointSize: 15
        font.bold: true
        font.family: "CMU Serif"

        color: Material.foreground
    }

    Text {
        id: body

        Layout.fillWidth: true

        text: {
            App.docs.locale
            App.docs.version
            return root.body ? root.body : App.docs.get(root.key)
        }

        wrapMode: Text.WordWrap
        color: Material.foreground
    }
}
