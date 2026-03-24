import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import SolTraceProto

Column {
    id: root
    spacing: 10

    property var model: App.sun.custom_sun_shape

    Row {
        spacing: 8

        GlassButton {
            label: "Load"
            onClicked: model.setData([
                {angle: -10, intensity: 0.1},
                {angle: -5,  intensity: 0.5},
                {angle: 0,   intensity: 1.0},
                {angle: 5,   intensity: 0.5},
                {angle: 10,  intensity: 0.1}
            ])
        }

        GlassButton {
            label: "Save"
            onClicked: console.log("Save data:", JSON.stringify(model.getData()))
        }

        GlassButton {
            label: "Copy"
            onClicked: model.copy_to_clipboard()
        }

        GlassButton {
            label: "Paste"
            onClicked: {
                model.paste_from_clipboard()
            }
        }

        GlassButton {
            label: "Clear All"
            onClicked: model.clear()
        }
    }

    Column {
        width: 460
        spacing: 0

        Row {
            id: tableHeader
            width: parent.width
            height: 40
            spacing: 0

            Rectangle {
                width: 60
                height: parent.height
                color: Qt.rgba(1, 1, 1, 0.05)
                border.width: 1
                border.color: Theme.lineColor

                Text {
                    anchors.centerIn: parent
                    text: ""
                    color: Theme.textColor
                    font.bold: true
                }
            }

            Rectangle {
                width: 170
                height: parent.height
                color: Qt.rgba(1, 1, 1, 0.05)
                border.width: 1
                border.color: Theme.lineColor

                Text {
                    anchors.centerIn: parent
                    text: "Angle (mrad)"
                    color: Theme.textColor
                    font.bold: true
                }
            }

            Rectangle {
                width: 170
                height: parent.height
                color: Qt.rgba(1, 1, 1, 0.05)
                border.width: 1
                border.color: Theme.lineColor

                Text {
                    anchors.centerIn: parent
                    text: "Intensity"
                    color: Theme.textColor
                    font.bold: true
                }
            }

            Rectangle {
                width: 60
                height: parent.height
                color: Qt.rgba(1, 1, 1, 0.05)
                border.width: 1
                border.color: Theme.lineColor

                Text {
                    anchors.centerIn: parent
                    text: ""
                    color: Theme.textColor
                    font.bold: true
                }
            }
        }

        Repeater {
            model: model

            CustomEmissionProfileTableRow {
                width: 460
                rowNumber: index + 1
                angleValue: model.angle
                intensityValue: model.intensity
                canDelete: model.count > 1
                table: root

                onAngleChanged: function(newValue) {
                    model.angle = newValue
                }

                onIntensityChanged: function(newValue) {
                    model.intensity = newValue
                }

                onRemoveClicked: {
                    model.remove(index)
                }
            }
        }

        // Add row button
        Rectangle {
            id: addRowButton
            width: parent.width
            height: 40
            color: mouseArea.containsMouse ? Theme.buttonHoverColor : Theme.buttonColor
            border.width: 1
            border.color: Theme.lineColor

            Behavior on color {
                ColorAnimation { duration: 100 }
            }

            Text {
                anchors.centerIn: parent
                text: "+ Add Row"
                color: Theme.textColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: model.append()
            }
        }
    }
}
