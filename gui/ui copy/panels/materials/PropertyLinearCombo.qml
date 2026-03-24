import QtQuick
import QtQuick.Layouts
//import QtQuick.Controls.Material
import SoltraceProto

Rectangle {
    id: root
    Layout.fillWidth: true

    property var model : []

    property int currentIndex : -1

    color: Material.backgroundColor

    implicitHeight: layout.implicitHeight + 24
    implicitWidth: layout.implicitWidth + 24

    radius: 14

    RowLayout {
        id: layout
        anchors.fill: parent

        spacing: 6

        Repeater {
            model: root.model

            Label {
                Layout.fillWidth: true
                text: modelData

                opacity: selected ? 1 : .75

                horizontalAlignment: Label.AlignHCenter

                property bool selected: root.currentIndex == index

                Rectangle {
                    //anchors.fill: parent
                    height: root.height
                    width: parent.width
                    opacity: selected

                    //color: Material.accentColor
                    color: "transparent"
                    border.color: Material.accentColor
                    border.width: 1
                    radius: 14
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: root.currentIndex = index
                }
            }
        }
    }

}
