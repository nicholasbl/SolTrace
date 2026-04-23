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

        STPropertyPanel {
            Layout.fillWidth: true

            collapsible: true
            title: "Ray Visibility"

            STPropertyLabel {
                text: "Event types"
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
                text: "Show percent"
            }

            STSpinBox {
                from: 0
                to: 100
                value: AppData.intersections.ray_geometry.show_percent

                Layout.fillWidth: true

                onValueModified: {
                    AppData.intersections.ray_geometry.show_percent = value
                }
            }
        }
    }
}
