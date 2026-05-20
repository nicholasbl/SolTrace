import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root

    Layout.fillHeight: true
    Layout.fillWidth: true


    ListView {
        id: log_list_view
        Layout.fillHeight: true
        Layout.fillWidth: true

        model: AppData.log_list

        clip: true

        spacing: 5

        onCountChanged: log_list_view.positionViewAtEnd()

        delegate: Label {
            required property string content

            width: ListView.view.width

            wrapMode: Label.WrapAtWordBoundaryOrAnywhere

            text: content
        }
    }
}