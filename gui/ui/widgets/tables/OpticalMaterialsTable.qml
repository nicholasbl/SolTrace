import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

Column {
    id: root
    spacing: 10
    property int numberOfPoints: 1
    property bool syncing: false
    width: parent.width

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

    Column {
        width: parent.width
        spacing: 0

        RowLayout {
            id: tableHeader
            width: parent.width
            height: 40
            spacing: 0

            Rectangle {
                Layout.preferredWidth: 50
                Layout.fillHeight: true
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
                Layout.preferredWidth: 50
                Layout.fillHeight: true
                color: Qt.rgba(1, 1, 1, 0.05)
                border.width: 1
                border.color: Theme.lineColor

                Text {
                    anchors.centerIn: parent
                    text: "No."
                    color: Theme.textColor
                    font.bold: true
                }
            }

            Rectangle {
                Layout.preferredWidth: 200
                Layout.fillHeight: true
                color: Qt.rgba(1, 1, 1, 0.05)
                border.width: 1
                border.color: Theme.lineColor

                Text {
                    anchors.centerIn: parent
                    text: "Name"
                    color: Theme.textColor
                    font.bold: true
                }
            }

            Rectangle {
                Layout.preferredWidth: 100
                Layout.fillHeight: true
                color: Qt.rgba(1, 1, 1, 0.05)
                border.width: 1
                border.color: Theme.lineColor

                Text {
                    anchors.centerIn: parent
                    text: "Side"
                    color: Theme.textColor
                    font.bold: true
                }
            }

            Repeater {
                model: ["ρ", "τ", "<em>n</em>", "σ<sub>slope</sub>", "σ<sub>spec</sub>", "ET", "ρ(θ)", "τ(θ)"]
                Rectangle {
                    required property string modelData
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Qt.rgba(1, 1, 1, 0.05)
                    border.width: 1
                    border.color: Theme.lineColor

                    Body {
                        anchors.centerIn: parent
                        text: parent.modelData
                        color: Theme.textColor
                        font.bold: true
                    }
                }
            }
        }

        Repeater {
            model: tableModel

            OpticalMaterialsTableRow {
                width: parent.width
                rowNumber: index + 1
                table: root
            }
        }

        // Add row button
        Rectangle {
            id: addRowButton
            width: parent.width
            height: 40
            color: Theme.buttonColor
            border.width: 1
            border.color: Theme.lineColor

            Behavior on color {
                ColorAnimation { duration: 100 }
            }

            RowLayout {
                anchors.centerIn: parent
                spacing: 10
                z: 1

                Text {
                    text: "+ Add Optical Material"
                    color: Theme.textColor
                    verticalAlignment: Text.AlignVCenter
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                    color: Theme.lineColor
                }

                Text {
                    text: "Template: "
                    color: Theme.textColor
                    verticalAlignment: Text.AlignVCenter
                }

                GlassComboBox {
                    id: materialTemplateCombo
                    Layout.preferredWidth: 150
                    model: ["None", "Example"]
                    labelFontSize: 13
                    enabled: true
                    z: 2
                }

                Item {
                    Layout.fillWidth: true
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                z: -5
                onClicked: {
                    root.addRow()
                }
            }
        }
    }
}
