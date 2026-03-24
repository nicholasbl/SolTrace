// Glass ComboBox
import QtQuick
import QtQuick.Controls.Material
import SolTraceProto

ComboBox {
    id: root

    //property var model
    //property int currentIndex: 0
    //property string currentText: model[currentIndex] || ""
    //property string optionFontFamily: ""
    //property var labelFontWeight: Font.Normal
    property int labelFontSize: 16
    property int optionFontSize: 14
    //property var source: null
    //property color borderColor: "transparent"
    //property color backgroundColor: "transparent"

    //signal activated(int index)

    implicitWidth: 200
    implicitHeight: 40

    function comboIndex(options, value) {
        if (!options || options.length === 0) {
            return -1
        }

        return options.indexOf(value)
    }


    delegate: ItemDelegate {
        id: delegate
        width: ListView.view.width
        height: 35

        opacity: delegate.containsMouse ? 1 : 0.9

        contentItem: STLabel {
            text: modelData ? modelData : model.display
            font.pointSize: root.optionFontSize
            anchors.fill: parent
            verticalAlignment: Qt.AlignVCenter
            anchors.leftMargin: 15
            elide: Label.ElideRight
        }

        highlighted: root.highlightedIndex === index
    }

    contentItem: STLabel {
        leftPadding: 5
        rightPadding: root.indicator.width + root.spacing

        text: root.displayText
        opacity: root.pressed ? 1.0 : 0.9
        font.pointSize: 14
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    indicator: STLabel {
        id: indicator
        text: "▼"
        font.pointSize: 14
        opacity: 0.7
        anchors.verticalCenter: parent.verticalCenter
        x: root.width - width - root.rightPadding

        transform: Rotation {
            angle: popup.visible ? 180 : 0
            origin.x: indicator.width /2
            origin.y: indicator.height/2
            Behavior on angle {
                NumberAnimation {
                    duration: 100
                }
            }
        }
    }

    // Main Button
    background: Rectangle {
        id: button
        anchors.left: parent.left
        anchors.right: parent.right
        color: "transparent"
        radius: 8
        border.color: Theme.lineColor
        border.width: 1


        implicitWidth: 200
        implicitHeight: 40
    }

    popup: Popup {
        y: root.height - 1
        width: root.width
        height: Math.min(contentItem.implicitHeight, root.Window.height - topMargin - bottomMargin)
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: root.popup.visible ? root.delegateModel : null
            currentIndex: root.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: GlassRectangle {
            radius: Theme.comboPopupRectRadius
        }
    }
}
