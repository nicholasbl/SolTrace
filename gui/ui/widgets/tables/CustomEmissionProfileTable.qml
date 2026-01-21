import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import SolTraceProto

Column {
    id: root
    spacing: 10
    property int numberOfPoints: 1
    property bool syncing: false

    onNumberOfPointsChanged: {
        if (!syncing) {
            syncModelToPointCount()
        }
    }

    Component.onCompleted: {
        syncModelToPointCount()
    }

    // This function ensures tableModel.count == numberOfPoints
    function syncModelToPointCount() {
        syncing = true

        // Add rows if we need more
        while (tableModel.count < numberOfPoints) {
            tableModel.append({
                angle: 0.0,
                intensity: 0.0
            })
        }

        // Remove rows if we have too many
        while (tableModel.count > numberOfPoints) {
            tableModel.remove(tableModel.count - 1)
        }

        syncing = false
    }

    function addRow() {
        numberOfPoints++
    }

    function removeRow(index) {
        if (numberOfPoints > 1) {
            numberOfPoints--
        }
    }

    function clearAll() {
        tableModel.clear()
        numberOfPoints = 1
    }

    function getTableData() {
        let data = []
        for (let i = 0; i < tableModel.count; i++) {
            data.push({
                angle: tableModel.get(i).angle,
                intensity: tableModel.get(i).intensity
            })
        }
        return data
    }

    function setTableData(data) {
        tableModel.clear()
        for (let i = 0; i < data.length; i++) {
            tableModel.append({
                angle: data[i].angle || 0.0,
                intensity: data[i].intensity || 0.0
            })
        }
        numberOfPoints = tableModel.count
    }

    function copyToClipboard() {
        let text = "Angle (mrad)\tIntensity\n"
        for (let i = 0; i < tableModel.count; i++) {
            let item = tableModel.get(i)
            text += item.angle + "\t" + item.intensity + "\n"
        }
    }

    function pasteFromClipboard(text) {
        let lines = text.split('\n')
        tableModel.clear()

        for (let i = 0; i < lines.length; i++) {
            if (lines[i].trim() === "") continue

            let values = lines[i].split(/[\t,]/)
            if (values.length >= 2) {
                tableModel.append({
                    angle: parseFloat(values[0]) || 0.0,
                    intensity: parseFloat(values[1]) || 0.0
                })
            }
        }
        numberOfPoints = tableModel.count
    }

    ListModel {
        id: tableModel
    }

    Row {
        spacing: 8

        GlassButton {
            label: "Load"
            onClicked: {
                root.setTableData([
                    {angle: -10, intensity: 0.1},
                    {angle: -5, intensity: 0.5},
                    {angle: 0, intensity: 1.0},
                    {angle: 5, intensity: 0.5},
                    {angle: 10, intensity: 0.1}
                ])
            }
        }

        GlassButton {
            label: "Save"
            onClicked: {
                console.log("Save data:", JSON.stringify(root.getTableData()))
            }
        }

        GlassButton {
            label: "Copy"
            onClicked: {
                root.copyToClipboard()
            }
        }

        GlassButton {
            label: "Paste"
            onClicked: {
                let sampleData = "-10\t0.1\n-5\t0.5\n0\t1.0\n5\t0.5\n10\t0.1"
                root.pasteFromClipboard(sampleData)
            }
        }

        GlassButton {
            label: "Clear All"
            onClicked: {
                root.clearAll()
            }
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
            model: tableModel

            CustomEmissionProfileTableRow {
                width: 460
                rowNumber: index + 1
                angleValue: model.angle
                intensityValue: model.intensity
                canDelete: tableModel.count > 1
                table: root

                onAngleChanged: function(newValue) {
                    tableModel.setProperty(index, "angle", newValue)
                }

                onIntensityChanged: function(newValue) {
                    tableModel.setProperty(index, "intensity", newValue)
                }

                onRemoveClicked: {
                    root.removeRow(index)
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
                onClicked: {
                    root.addRow()
                }
            }
        }
    }
}
