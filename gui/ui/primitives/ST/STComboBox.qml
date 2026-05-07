pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

ComboBox {
    id: root
    Layout.fillWidth: true

    background: WellRectangle {
        implicitWidth: 80
        implicitHeight: 32
        radius: height / 2
    }

    indicator: Label {
        font.family: "Font Awesome 7 Free"
        text: "\uf078"

        x: root.width - width - root.rightPadding
        y: root.topPadding + (root.availableHeight - height) / 2
        width: 12
        height:12
    }

    popup: Popup {
        //y: root.height - 1
        width: root.width
        height: Math.min(contentItem.implicitHeight, root.Window.height - topMargin - bottomMargin)
        padding: 2

        //onOpened: console.log("popup opened")
        //onClosed: console.log("popup closed")

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: root.popup.visible ? root.delegateModel : null
            currentIndex: root.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }

            delegate: STItemDelegate {
                id: delegate
                width: ListView.view.width

                required property var model
                required property int index

                text: root.textAt(index)
            }
        }

        background: Rectangle {
            color: Qt.alpha(Material.backgroundColor, .90)
            border.color: Material.frameColor
            radius: 10
        }
    }
}
