import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

STPopup {
    id: root

    property var current_db: AppData.file_source.current_database
    property int selected_result_index: -1

    enable_shadow: true

    function formatRayCount(count) {
        return Number(count).toLocaleString(Qt.locale(), "f", 0) + " rays"
    }

    function showSceneMode() {
        App.view.simulation_content_view = false
    }

    function showResultMode() {
        App.view.simulation_content_view = true
        App.view.workflow_phase = 2
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        STComboBar {
            id: mode_bar
            Layout.fillWidth: true

            iconModel: ["\uf1c0", "\uf1da"]
            model: ["Scenes", "Results"]
            currentIndex: App.view.simulation_content_view ? 1 : 0

            onCurrentIndexChanged: {
                if (currentIndex === 1) {
                    root.showResultMode()
                } else {
                    root.showSceneMode()
                }
            }
        }

        Rectangle {
            color: Material.dividerColor
            Layout.preferredHeight: 1
            Layout.fillWidth: true
            Layout.leftMargin: 3
            Layout.rightMargin: 3
        }

        StackLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            currentIndex: mode_bar.currentIndex

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 8

                Label {
                    Layout.fillWidth: true

                    text: "Available Scenes"
                    font.bold: true
                }

                ListView {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    model: AppData.file_source
                    clip: true

                    delegate: STItemDelegate {
                        id: db_delegate

                        required property int index
                        required property var database

                        text: database.name

                        font.pointSize: 20

                        highlighted: root.current_db === database

                        onClicked: {
                            AppData.file_source.set_current(index)
                            root.showSceneMode()
                        }
                    }
                }

                Rectangle {
                    color: Material.dividerColor
                    Layout.preferredHeight: 1
                    Layout.fillWidth: true
                    Layout.leftMargin: 3
                    Layout.rightMargin: 3
                }

                Label {
                    text: "Current Scene"
                    font.bold: true
                }

                RowLayout {
                    enabled: !!root.current_db

                    Label {
                        font.family: "Font Awesome 7 Free"
                        font.pointSize: 16

                        text: "\uf303"
                    }

                    STTextField {
                        text: root.current_db ? root.current_db.name : ""
                        Layout.fillWidth: true

                        onTextChanged: {
                            if (root.current_db) {
                                root.current_db.name = text
                            }
                        }
                    }
                }

                RowLayout {
                    STIconButton {
                        text: "\uf2ed"
                        toolTip: "Delete Scene"
                        onClicked: AppData.file_source.delete_current()
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    STIconButton {
                        text: "\uf055"
                        toolTip: "New Scene"

                        onClicked: new_name_pop.open()

                        STDialog {
                            id: new_name_pop

                            modal: false

                            STTextField {
                                id: text_input
                                placeholderText: "New scene name..."

                                onAccepted: new_name_pop.accept()
                            }

                            standardButtons: Dialog.Ok | Dialog.Cancel

                            onAccepted: AppData.file_source.append_new(text_input.text)
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        Layout.fillWidth: true
                        text: "Simulation Results"
                        font.bold: true
                    }

                    STButton {
                        enabled: AppData.simulation.results.rowCount() > 0
                        text: "Unfreeze to Scene"
                        text_icon: "\uf2f1"

                        onClicked: {
                            AppData.simulation.duplicate_current_result_for_edit()
                            root.showSceneMode()
                        }
                    }
                }

                Label {
                    Layout.fillWidth: true
                    visible: AppData.simulation.results.rowCount() === 0
                    text: "No simulation results yet."
                    opacity: 0.7
                    horizontalAlignment: Text.AlignHCenter
                }

                ListView {
                    id: result_list
                    property int previousCount: 0

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    clip: true
                    model: AppData.simulation.results

                    onCountChanged: {
                        if (count > previousCount) {
                            currentIndex = count - 1
                            root.selected_result_index = currentIndex
                        } else if (currentIndex >= count) {
                            currentIndex = count - 1
                            root.selected_result_index = currentIndex
                        }
                        previousCount = count
                    }

                    delegate: ItemDelegate {
                        id: resultDelegate
                        required property int index

                        required property string name
                        required property var when
                        required property var ray_count

                        width: result_list.width
                        implicitHeight: 64
                        highlighted: ListView.isCurrent ? ListView.isCurrent : false
                        hoverEnabled: true

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
                                text: root.formatRayCount(resultDelegate.ray_count)
                                opacity: 0.85
                                horizontalAlignment: Text.AlignRight
                            }

                            STIconButton {
                                Layout.alignment: Qt.AlignVCenter
                                text: "\uf019"
                                iconSize: 12
                                toolTip: "Export Result"
                                onClicked: AppData.simulation.export_result(resultDelegate.index)
                            }

                            STIconButton {
                                Layout.alignment: Qt.AlignVCenter
                                text: "\uf1f8"
                                iconSize: 12
                                toolTip: "Delete Result"
                                onClicked: AppData.simulation.delete_result(resultDelegate.index)
                            }
                        }

                        onClicked: {
                            result_list.currentIndex = index
                            root.selected_result_index = index
                            AppData.simulation.select_result(index)
                            root.showResultMode()
                        }
                    }
                }
            }
        }
    }
}
