import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

Flickable {
    id: root
    property var left_panel_size: App.view.left_panel.size
    property var intersections_module : AppData.intersections
    property bool singleColumn: App.view.left_panel.size === PanelData.Small
    property int labelAlignment: (singleColumn ? Qt.AlignLeft : Qt.AlignRight) | Qt.AlignVCenter

    function formatRayCount(count) {
        return Number(count).toLocaleString(Qt.locale(), "f", 0) + " rays"
    }

    function visibleRayCount() {
        const available = AppData.intersections.ray_geometry.available_rays
        return Math.round(available *
                          AppData.intersections.ray_geometry.show_percent / 100.0)
    }

    function setVisibleRayCount(count) {
        const available = AppData.intersections.ray_geometry.available_rays
        AppData.intersections.ray_geometry.show_percent =
                available > 0 ? count * 100.0 / available : 0.0
    }

    contentWidth: width
    contentHeight: content_column.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    ColumnLayout {
        id: content_column
        width: root.width

        InlineDocumentation {
            key: "analyze.intersections"
            target: App.view.left_panel
        }

        Label {
            Layout.fillWidth: true
            Layout.columnSpan: 2
            text: root.formatRayCount(
                      AppData.intersections.ray_geometry.available_rays)
            opacity: 0.75
            horizontalAlignment: Text.AlignHCenter
        }

        STPropertyPanel {
            Layout.fillWidth: true
            columns: root.singleColumn ? 1 : 2

            collapsible: true
            title: "Ray Visibility"

            STPropertyLabel {
                text: "Filter types"
                Layout.alignment: root.labelAlignment
            }

            STButton {
                Layout.fillWidth: true
                text: AppData.intersections.ray_geometry.event_include.length > 0
                      ? AppData.intersections.ray_geometry.event_include.join(", ")
                      : "None"

                onClicked: ray_filter_popup.open_with(
                               AppData.intersections.ray_geometry.event_include)

                RayFilterPopup {
                    id: ray_filter_popup

                    onModified: function(filter) {
                        AppData.intersections.ray_geometry.event_include = filter
                    }
                }
            }

            STPropertyLabel {
                text: "Show"
                Layout.alignment: root.labelAlignment
            }

            STSwitch {
                checked: AppData.view.show_intersections

                onToggled: AppData.view.show_intersections = checked
            }

            STDoubleSpinBox {
                Layout.columnSpan: 2
                from: 0.0
                to: 100.0
                value: AppData.intersections.ray_geometry.show_percent
                stepSize: 1.0
                Layout.fillWidth: true
                onValueModified: {
                    AppData.intersections.ray_geometry.show_percent = value
                }
                suffix: "%"
            }

            STDoubleSpinBox {
                Layout.columnSpan: 2
                from: 0.0
                to: AppData.intersections.ray_geometry.available_rays
                value: root.visibleRayCount()
                stepSize: 1000
                decimals: 0
                Layout.fillWidth: true
                onValueModified: root.setVisibleRayCount(value)
                suffix: "rays"
            }
        }
    }
}
