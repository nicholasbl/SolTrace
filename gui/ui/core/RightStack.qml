import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ShadowedGlassRectangle {
    id: right_stack

    Label {
        id: info_row
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        height: 34

        text: "Scripting"
        font.pointSize: 18
        font.bold: true

        elide: Label.ElideRight

        horizontalAlignment: Label.AlignHCenter
        verticalAlignment: Label.AlignVCenter
    }

    // need this?
    StackLayout {
        id: script_stack

        anchors.top: info_row.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10

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
    }
}
