import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import SolTraceProto

GridLayout {
    id: root
    width: parent.width
    rows: 2
    columns: 12
    rowSpacing: 0
    columnSpacing: 0

    property int rowNumber: 1
    property string materialName: ""
    property bool canDelete: true
    property var table: null
    property bool selected: false

    // Organized side properties
    property QtObject frontSide: QtObject {
        property bool enabled: false
        property real reflectance: 0.0
        property real transmittance: 0.0
        property real refractiveIndex: 1.0
        property real slopeError: 0.0
        property real specError: 0.0
        property string errorType: "Gaussian"
        property string reflectanceFunction: ""
        property string transmittanceFunction: ""
    }

    property QtObject backSide: QtObject {
        property bool enabled: false
        property real reflectance: 0.0
        property real transmittance: 0.0
        property real refractiveIndex: 1.0
        property real slopeError: 0.0
        property real specError: 0.0
        property string errorType: "Gaussian"
        property string reflectanceFunction: ""
        property string transmittanceFunction: ""
    }

    signal nameChanged(string newValue)
    signal removeClicked()

    // Side definitions
    property var sides: [
        { row: 0, name: "Front", sideObj: root.frontSide, bgColor: "transparent" },
        { row: 1, name: "Back", sideObj: root.backSide, bgColor: Theme.shadedRowColor }
    ]

    // Column definitions for numeric fields
    property var numericColumns: [
        { col: 4, prop: "reflectance", min: 0.0, max: 1.0, decimals: 3 },
        { col: 5, prop: "transmittance", min: 0.0, max: 1.0, decimals: 3 },
        { col: 6, prop: "refractiveIndex", min: 1.0, max: 3.0, decimals: 3 },
        { col: 7, prop: "slopeError", min: 0.0, max: 10.0, decimals: 3 },
        { col: 8, prop: "specError", min: 0.0, max: 10.0, decimals: 3 }
    ]

    property var functionColumns: [
        { col: 10, prop: "reflectanceFunction" },
        { col: 11, prop: "transmittanceFunction" }
    ]

    // Checkbox (spans 2 rows)
    Rectangle {
        Layout.row: 0
        Layout.column: 0
        Layout.rowSpan: 2
        Layout.preferredWidth: 50
        Layout.preferredHeight: 80
        color: "transparent"
        border.width: 1
        border.color: Theme.lineColor

        CheckBox {
            anchors.centerIn: parent
            checked: root.selected
            radius: 5
            width: 20
            height: width
            onCheckedChanged: root.selected = checked
        }
    }

    // Row number (spans 2 rows)
    Rectangle {
        Layout.row: 0
        Layout.column: 1
        Layout.rowSpan: 2
        Layout.preferredWidth: 50
        Layout.preferredHeight: 80
        color: "transparent"
        border.width: 1
        border.color: Theme.lineColor

        Text {
            anchors.centerIn: parent
            text: root.rowNumber
            color: Theme.textColor
        }
    }

    // Name (spans 2 rows)
    Rectangle {
        Layout.row: 0
        Layout.column: 2
        Layout.rowSpan: 2
        Layout.preferredWidth: 200
        Layout.preferredHeight: 80
        color: "transparent"
        border.width: 1
        border.color: Theme.lineColor

        TextField {
            anchors.centerIn: parent
            width: parent.width - 10
            height: 30
            text: root.materialName ? root.materialName : "Material " + root.rowNumber
            horizontalAlignment: Text.AlignLeft
            leftPadding: 5
            background: Rectangle { color: "transparent" }
            color: Theme.textColor
            selectByMouse: true
            onEditingFinished: root.nameChanged(text)
        }
    }

    // OUTER REPEATER - Sides (Front and Back)
    Repeater {
        model: root.sides

        // Side toggle button
        Rectangle {
            required property var modelData

            Layout.row: modelData.row
            Layout.column: 3
            Layout.preferredWidth: 100
            Layout.preferredHeight: 40
            color: modelData.bgColor
            border.width: 1
            border.color: Theme.lineColor

            Row {
                anchors.centerIn: parent
                spacing: 5

                Text {
                    text: parent.parent.modelData.name
                    color: Theme.textColor
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }

                GlassButton {
                    width: 20
                    height: 20
                    label: parent.parent.modelData.sideObj.enabled ? "×" : "+"
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: parent.parent.modelData.sideObj.enabled = !parent.parent.modelData.sideObj.enabled
                }
            }
        }
    }

    // NESTED REPEATER - Sides × Numeric Columns
    Repeater {
        model: root.sides

        Repeater {
            required property var modelData
            property var side: modelData

            model: root.numericColumns

            Rectangle {
                required property var modelData
                property var sideData: side

                Layout.row: sideData.row
                Layout.column: modelData.col
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                color: sideData.bgColor
                border.width: 1
                border.color: Theme.lineColor

                NumberField {
                    anchors.centerIn: parent
                    fieldWidth: parent.width - 10
                    visible: parent.sideData.sideObj.enabled
                    enabled: parent.sideData.sideObj.enabled
                    minValue: parent.modelData.min
                    maxValue: parent.modelData.max
                    decimals: parent.modelData.decimals
                    value: parent.sideData.sideObj[parent.modelData.prop]
                    textField.borderColor: "transparent"
                    textField.activeFocusBorderColor: "transparent"

                    onValueChanged: {
                        parent.sideData.sideObj[parent.modelData.prop] = value
                    }
                }
            }
        }
    }

    // NESTED REPEATER - Sides × Error Type
    Repeater {
        model: root.sides

        Rectangle {
            required property var modelData

            Layout.row: modelData.row
            Layout.column: 9
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: modelData.bgColor
            border.width: 1
            border.color: Theme.lineColor

            GlassComboBox {
                anchors.centerIn: parent
                width: parent.width - 10
                height: parent.height - 10
                visible: parent.modelData.sideObj.enabled
                enabled: parent.modelData.sideObj.enabled
                model: ["Gaussian", "Pillbox", "Diffuse"]
                labelFontSize: Theme.controlLabelSize
                currentIndex: parent.modelData.sideObj.errorType === "Pillbox" ? 1 : 0
                onActivated: parent.modelData.sideObj.errorType = currentText
            }
        }
    }

    // NESTED REPEATER - Sides × Function Columns
    Repeater {
        model: root.sides

        Repeater {
            required property var modelData
            property var side: modelData

            model: root.functionColumns

            Rectangle {
                required property var modelData
                property var sideData: side

                Layout.row: sideData.row
                Layout.column: modelData.col
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                color: sideData.bgColor
                border.width: 1
                border.color: Theme.lineColor

                TextField {
                    anchors.centerIn: parent
                    width: parent.width - 10
                    height: parent.height - 10
                    visible: parent.sideData.sideObj.enabled
                    enabled: parent.sideData.sideObj.enabled
                    text: parent.sideData.sideObj[parent.modelData.prop]
                    horizontalAlignment: Text.AlignHCenter
                    background: Rectangle { color: "transparent" }
                    color: Theme.textColor
                    selectByMouse: true
                    placeholderText: ""
                    onEditingFinished: parent.sideData.sideObj[parent.modelData.prop] = text
                }
            }
        }
    }
}
