import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

STPopup {
    enable_shadow: true

    ColumnLayout {
        anchors.fill: parent

        Label {
            Layout.fillWidth: true

            text: "Available Databases"
        }

        ListView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            model: AppData.file_source

            delegate: STItemDelegate {
                required property int index
                required property var database

                text: database.name

                highlighted: AppData.file_source.current_database === database

                onClicked: {
                    AppData.file_source.set_current(index)
                }
            }
        }
    }
}
