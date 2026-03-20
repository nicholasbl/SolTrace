import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

GridLayout {
    id: table

    property var columnHeaders: []
    property var columnKeys: []
    property var columnWidths: []
    property var rowData: []
    property int symbolSize: 17
    property bool firstColumnBold: true
    property int padding: 10
    property var columnsCentered: []
    property var columnsBolded: []
    property var columnsFontSize: []
    property var columnsFontFamily: []

    width: parent.width
    rowSpacing: 0
    columnSpacing: 0

    rows: 3 + rowData.length * 2  // top divider + header + header divider + (data row + divider) * n
    columns: columnHeaders.length * 2 + 1  // (divider + column) * n + final divider

    // Top divider
    Rectangle {
        Layout.row: 0
        Layout.column: 0
        Layout.columnSpan: table.columns
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Theme.lineColor
    }

    // Vertical dividers (all rows)
    Repeater {
        model: columnHeaders.length + 1
        Rectangle {
            Layout.row: 1
            Layout.column: index * 2
            Layout.rowSpan: table.rows - 1  // Skip top divider row
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            color: Theme.lineColor
        }
    }

    // Header cells
    Repeater {
        model: table.columnHeaders
        Body {
            Layout.row: 1
            Layout.column: index * 2 + 1
            Layout.preferredWidth: {
                var width = table.columnWidths[index]
                return width > 0 ? width : -1
            }
            Layout.fillWidth: {
                var width = table.columnWidths[index]
                return width === undefined || width === -1
            }
            text: modelData
            horizontalAlignment: index === 0 && table.firstColumnBold ? Text.AlignHCenter : Text.AlignLeft
            padding: table.padding
            font.weight: Font.Bold
        }
    }

    // Header divider
    Rectangle {
        Layout.row: 2
        Layout.column: 0
        Layout.columnSpan: table.columns
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Theme.lineColor
    }

    // Data cells
    Repeater {
        model: table.rowData.length * table.columnHeaders.length
        Body {
            property int rowIndex: Math.floor(index / table.columnHeaders.length)
            property int colIndex: index % table.columnHeaders.length
            property var rowData: table.rowData[rowIndex]
            property string key: table.columnKeys[colIndex]

            Layout.row: rowIndex * 2 + 3
            Layout.column: colIndex * 2 + 1
            Layout.preferredWidth: {
                var width = table.columnWidths[colIndex]
                return width > 0 ? width : -1
            }
            Layout.fillWidth: {
                var width = table.columnWidths[colIndex]
                return width === undefined || width === -1
            }
            padding: table.padding
            text: rowData[key] || ""
            font.family: (table.columnsFontFamily && table.columnsFontFamily[colIndex]) ? table.columnsFontFamily[colIndex] : font.family
            font.pointSize: (table.columnsFontSize && table.columnsFontSize[colIndex]) ? table.columnsFontSize[colIndex] : font.pointSize
            font.weight: (table.columnsBolded && table.columnsBolded[colIndex]) ? Font.Bold : Font.Normal
            horizontalAlignment: (table.columnsCentered && table.columnsCentered[colIndex]) ? Text.AlignHCenter : Text.AlignLeft
        }
    }

    // Row dividers (between data rows)
    Repeater {
        model: table.rowData.length
        Rectangle {
            Layout.row: index * 2 + 4
            Layout.column: 0
            Layout.columnSpan: table.columns
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.lineColor
        }
    }
}
