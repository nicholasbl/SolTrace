import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

STPopup {
    id: root

    property var current_db: AppData.file_source.current_database

    enable_shadow: true

    ColumnLayout {
        anchors.fill: parent

        Label {
            Layout.fillWidth: true

            text: "Available Databases"
        }

        Rectangle {
            color: Material.dividerColor
            Layout.preferredHeight: 1
            Layout.fillWidth: true
            Layout.leftMargin: 3
            Layout.rightMargin: 3
        }

        ListView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            model: AppData.file_source

            delegate: STItemDelegate {
                id: db_delegate

                required property int index
                required property var database

                text: database.name

                font.pointSize: 20

                highlighted: root.current_db === database

                onClicked: {
                    AppData.file_source.set_current(index)
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

        RowLayout {
            Label {
                font.family: "Font Awesome 7 Free"

                font.pointSize: 16

                text: "\uf303"
            }

            STTextField {
                text: current_db.name
                Layout.fillWidth: true

                onTextChanged: current_db.name = text
            }
        }

        RowLayout {
            STIconButton {
                text: "\uf2ed"
                onClicked: AppData.file_source.delete_current()
            }

            Item {
                Layout.fillWidth: true
            }

            STIconButton {
                text: "\uf055"

                onClicked: new_name_pop.open()

                STDialog {
                    id: new_name_pop

                    modal: false

                    STTextField {
                        id: text_input
                        placeholderText: "New database name..."

                        onAccepted: new_name_pop.accept()
                    }

                    standardButtons: Dialog.Ok | Dialog.Cancel

                    onAccepted: AppData.file_source.append_new(text_input.text)
                }

            }
        }
    }
}
