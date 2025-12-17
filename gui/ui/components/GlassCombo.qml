// Glass ComboBox
import QtQuick
import QtQuick.Controls

Item {
    id: root

    property var model: []
    property int currentIndex: 0
    property string currentText: model[currentIndex] || ""
    property color textColor: "white"
    property string labelFontFamily: ""
    property string optionFontFamily: ""
    property var labelFontWeight: Font.Normal
    property int labelFontSize: 16
    property int optionFontSize: 14
    property var source: null

    signal activated(int index)

    implicitWidth: 200
    implicitHeight: 35

    // Main Button
    Rectangle {
        id: button
        anchors.fill: parent
        color: "transparent"
        radius: 5

        Row {
            anchors.fill: parent
            anchors.leftMargin: 15
            anchors.rightMargin: 15
            spacing: 10

            Text {
                text: root.currentText
                color: root.textColor
                font.family: root.labelFontFamily
                font.pixelSize: root.labelFontSize
                font.weight: root.labelFontWeight
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - indicator.width - parent.spacing
                elide: Text.ElideRight
            }

            Text {
                id: indicator
                text: popup.visible ? "▲" : "▼"
                color: root.textColor
                font.pixelSize: 10
                opacity: 0.7
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            id: buttonMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: popup.visible = !popup.visible
        }
    }

    // Dropdown Popup
    Popup {
        id: popup
        y: parent.height + 2
        width: parent.width
        padding: 0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: GlassRect {
            id: background
            color: "transparent"
            source: root.source
        }

        contentItem: ScrollView {
            implicitHeight: Math.min(listView.contentHeight, 300)
            clip: true

            ListView {
                id: listView
                model: root.model
                currentIndex: root.currentIndex

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 35
                    color: itemMouse.containsMouse ? Qt.rgba(255, 255, 255, 0.2) : "transparent"
                    opacity: itemMouse.containsMouse ? 1 : 0.9
                    radius: background.radius

                    Text {
                        text: modelData
                        color: root.textColor
                        font.family: root.optionFontFamily
                        font.pixelSize: root.optionFontSize
                        anchors.left: parent.left
                        anchors.leftMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    MouseArea {
                        id: itemMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.currentIndex = index
                            root.activated(index)
                            popup.close()
                        }
                    }
                }
            }
        }
    }
}
