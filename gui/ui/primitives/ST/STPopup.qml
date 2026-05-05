import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Popup {
    id: root

    background: Rectangle {
        radius: 14

        color: Qt.alpha(Material.backgroundColor, .90)
    }

    margins: 1
    padding: 8
}
