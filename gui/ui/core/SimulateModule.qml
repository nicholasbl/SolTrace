import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root
    property int size_class

    STButton {
        text: "Enqueue Job"
        text_icon: "\uf0da"
    }
}
