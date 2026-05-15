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

        Layout.fillHeight: true
        Layout.fillWidth: true
        clip: true
        model: root.simulation ? root.simulation.results : null

        onCountChanged: {
            if (count > 0) currentIndex = count - 1
        }

        delegate: STItemDelegate {
            required property int index

            required property string name
            required property var when
            required property int ray_count

            text: name + " @ " + Qt.formatDateTime(when, "yyyy-MM-dd hh:mm:ss")

            onClicked: {
                history_list.currentIndex = index
                root.simulation.select_result(index)
            }

        }

    }
}
