import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

Flickable {
    id: root
    property var left_panel_size: App.view.left_panel.size
    property var flux_module : AppData.flux

    contentWidth: width
    contentHeight: content_column.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    ColumnLayout {
        id: content_column
        InlineDocumentation {
            key: "analyze.flux"
            target: App.view.left_panel
        }

        STPropertyPanel {
            Layout.fillWidth: true

            collapsible: false
            title: "Compute Map"

            ListView {
                Layout.columnSpan: 2
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.preferredHeight: 250
                clip: true

                model: AppData.flux.entity_model

                ScrollIndicator.vertical: ScrollIndicator { }

                delegate: ItemDelegate {
                    id: delegate
                    required property string name
                    required property var entity
                    text: delegate.name
                    //highlighted: isCurrent
                    width: parent ? parent.width : implicitWidth

                    background: Rectangle {
                        implicitHeight: 24
                        implicitWidth: 100
                        opacity: enabled ? 1 : 0.3
                        color: parent.down ? Material.rippleColor : "transparent"
                    }

                    onClicked: {
                        AppData.flux.current_entity = delegate.entity
                    }
                }
            }

            STButton {
                Layout.fillWidth: true
                Layout.columnSpan: 2

                text: "Enqueue Job"
                text_icon: "\uf0da"

                onClicked: {
                    AppData.flux.start_generate()
                }
            }
        }
    }
}
