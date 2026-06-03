import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root
    readonly property var simulation: AppData.simulation

    Layout.fillHeight: true
    Layout.fillWidth: true

    ListView {
        id: history_list
        property int previousCount: 0

        Layout.fillHeight: true
        Layout.fillWidth: true
        clip: true
        model: root.simulation ? root.simulation.results : null

        onCountChanged: {
            if (count > previousCount) {
                currentIndex = count - 1
            } else if (currentIndex >= count) {
                currentIndex = count - 1
            }
            previousCount = count
        }

        delegate: ItemDelegate {
            id: resultDelegate
            required property int index

            required property string name
            required property var when
            required property var ray_count

            width: history_list.width
            implicitHeight: 64
            highlighted: ListView.isCurrent ? ListView.isCurrent : false
            hoverEnabled: true

            function formatRayCount(count) {
                return Number(count).toLocaleString(Qt.locale(), "f", 0) + " rays"
            }

            background: Rectangle {
                opacity: resultDelegate.enabled ? 1 : 0.3
                color: resultDelegate.down
                       ? Material.rippleColor
                       : resultDelegate.highlighted
                         ? Material.highlightedRippleColor
                         : resultDelegate.hovered
                           ? Qt.alpha(Material.highlightedRippleColor, 0.35)
                           : "transparent"
                radius: 14
            }

            contentItem: RowLayout {
                spacing: 10

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                    spacing: 2

                    Label {
                        Layout.fillWidth: true
                        text: resultDelegate.name
                        font.bold: true
                        elide: Text.ElideRight
                    }

                    Label {
                        Layout.fillWidth: true
                        text: Qt.formatDateTime(resultDelegate.when,
                                                "yyyy-MM-dd hh:mm:ss")
                        opacity: 0.7
                        elide: Text.ElideRight
                    }
                }

                Label {
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    text: resultDelegate.formatRayCount(resultDelegate.ray_count)
                    opacity: 0.85
                    horizontalAlignment: Text.AlignRight
                }

                STIconButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "\uf019"
                    iconSize: 12
                    toolTip: "Export Result"
                    onClicked: root.simulation.export_result(resultDelegate.index)
                }

                STIconButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "\uf1f8"
                    iconSize: 12
                    toolTip: "Delete Result"
                    onClicked: root.simulation.delete_result(resultDelegate.index)
                }
            }

            onClicked: {
                history_list.currentIndex = index
                root.simulation.select_result(index)
            }

        }

    }
}
