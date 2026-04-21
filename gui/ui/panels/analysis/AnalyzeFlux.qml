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
        width: root.width

        InlineDocumentation {
            key: "analyze.flux"
            target: App.view.left_panel
        }

        STPropertyPanel {
            Layout.fillWidth: true

            collapsible: true
            title: "Compute Flux Map"

            Image {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.preferredHeight: width

                visible: AppData.flux.current_image.length

                source: map_selector.currentIndex == 0 ?
                            AppData.flux.current_image
                          :
                            AppData.flux.current_image + "_point_map"
                fillMode: Image.PreserveAspectFit

                mipmap: true

                STIconButton {
                    text: "\uf019"

                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.margins: 10
                }
            }

            STComboBar {
                id: map_selector

                visible: AppData.flux.current_image.length

                Layout.columnSpan: 2
                Layout.fillWidth: true
                model: ["Bins", "Points"]
            }

            ListView {
                visible: false
                Layout.columnSpan: 2
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.preferredHeight: 200
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

        STPropertyPanel {
            Layout.fillWidth: true

            collapsible: true
            title: "Flux Volume"

            STPropertyLabel {
                text: "Grid Resolution"
            }

            STSpinBox {
                id: resolution_spin
                Layout.fillWidth: true

                value: 512
                from: 64
                to: 2048
            }

            STButton {
                enabled: !AppData.flux.ray_volume_flux_in_progress
                Layout.fillWidth: true
                Layout.columnSpan: 2

                text: "Start Raster"
                text_icon: "\uf0da"

                onClicked: {
                    AppData.flux.start_generate_volume_flux(resolution_spin.value)
                }
            }
        }

        STPropertyPanel {
            Layout.fillWidth: true

            collapsible: true
            title: "Flux Isosurface"

            STPropertyLabel {
                text: "Isovalue"
            }

            STDoubleSpinBox {
                id: iso_spin
                Layout.fillWidth: true

                value: 0.90
                from: 0.0
                stepSize: .01
                to: 1.0
            }

            STButton {
                enabled: !AppData.flux.ray_volume_flux_in_progress
                Layout.fillWidth: true
                Layout.columnSpan: 2

                text: "Generate Surface"
                text_icon: "\uf0da"

                onClicked: {
                    AppData.flux.start_generate_isosurface(iso_spin.value)
                }
            }
        }
    }
}
