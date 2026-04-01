import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ShadowedGlassRectangle {
    id: right_stack

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        Label {
            id: info_row
            Layout.fillWidth: true

            text: ["Scripting", "Help", "Export"][App.view.right_panel_section]
            font.pointSize: 16
            font.family: "CMU Serif"
            font.bold: true

            elide: Label.ElideRight

            horizontalAlignment: Label.AlignHCenter
            verticalAlignment: Label.AlignVCenter
        }

        STComboBar {
            id: section
            Layout.fillWidth: true

            currentIndex: App.view.right_panel_section
            onCurrentIndexChanged: App.view.right_panel_section = currentIndex

            fontFamily: App.view.right_panel_size < 1 ? "Font Awesome 7 Free" : ""

            model: App.view.right_panel_size < 1 ?
                        ["\uf06a", "\uf06a", "\uf06a"]
                      :
                        ["Scripting", "Help", "Export"]
        }

        // need this?
        StackLayout {
            id: script_stack
            currentIndex: App.view.right_panel_section

            Layout.fillWidth: true

            ColumnLayout {
                ScrollView {
                    id: left_scroll
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    contentWidth: availableWidth
                    STPropertyPanel {
                        anchors.fill: parent

                        title: "Script Properties"
                        collapsible: true

                        STPropertyLabel {
                            text: "Property A"
                        }

                        STSpinBox {
                            Layout.fillWidth: true
                            decimals: 2
                        }
                    }
                }

                STButton {
                    text: "Run"
                    Layout.fillWidth: true
                }

                TextArea {
                    Layout.fillWidth: true
                    Layout.preferredHeight: right_stack.height / 2
                    text: "Script goes here"
                }
            }

            Item {}
            Item {}
        }
    }
}
