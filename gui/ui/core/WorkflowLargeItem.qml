import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

Rectangle {
    id: root

    property string icon
    property string title
    property string value
    property bool active: false
    property bool value_clickable: true

    signal activated()
    signal valueClicked()

    Layout.preferredWidth: 170
    Layout.preferredHeight: 84

    radius: 8
    color: item_mouse.containsMouse ? Material.rippleColor
                                    : Material.dividerColor
    border.width: active ? 1 : 0
    border.color: Material.frameColor

    MouseArea {
        id: item_mouse

        anchors.fill: parent
        hoverEnabled: true

        onClicked: root.activated()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        RowLayout {
            Layout.fillWidth: true

            Label {
                font.family: "Font Awesome 7 Free"
                text: root.icon
            }

            Label {
                Layout.fillWidth: true
                text: root.title
                font.bold: true
                elide: Text.ElideRight
            }
        }

        STClickableLabel {
            Layout.fillWidth: true
            text: root.value
            elide: Text.ElideRight
            opacity: root.value_clickable ? 0.9 : 0.7
            borderWidth: 0
            enabled: root.value_clickable

            onClicked: root.valueClicked()
        }
    }
}
