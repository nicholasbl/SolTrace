import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ShadowedGlassRectangle {
    id: root
    required property int available_width

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 5

            Label {
                id: info_row
                Layout.fillWidth: true

                text: ["Scripting", "Help", "Export"][App.view.right_panel_section]
                font.pointSize: 16
                font.family: "CMU Serif"
                font.bold: true

                elide: Label.ElideRight
                // horizontalAlignment: App.view.right_panel.size == PanelData.Small ? Label.AlignLeft : Label.AlignHCenter
                verticalAlignment: Label.AlignVCenter
            }

            PanelButtons {
                id: panel_buttons
                target: App.view.right_panel
                otherTarget: App.view.left_panel
                available_width: root.available_width
                is_right_panel: true
            }
        }

        STComboBar {
            id: section
            Layout.fillWidth: true

            currentIndex: App.view.right_panel_section
            onCurrentIndexChanged: App.view.right_panel_section = currentIndex

            fontFamily: App.view.right_panel.size < 1 ? "Font Awesome 7 Free" : ""

            model: App.view.right_panel.size < 1 ?
                        ["\uf1c9", "\ue4e3", "\uf019"]
                      :
                        ["Scripting", "Help", "Export"]
        }

        // need this?
        StackLayout {
            id: script_stack
            currentIndex: App.view.right_panel_section

            Layout.fillWidth: true
            Layout.fillHeight: true

            ScriptInterface {}

            Item {}
            Item {}
        }
    }
}
