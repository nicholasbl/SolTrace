import QtQuick 2.15
import QtQuick.Layouts
import SolTraceProto

Item {
    id: root
    property var source: null
    property int sidebarWidth: 300
    property int headerHeight: 50
    anchors.fill: parent
    anchors.margins: 20

    GlassRect {
        anchors.fill: parent
        sourceItem: root.source

        GridLayout {
            id: contentGrid
            anchors.fill: parent
            columns: 3
            rows: 3
            columnSpacing: 0
            rowSpacing: 0

            Item {
                id: sidebarHeader
                Layout.row: 0
                Layout.column: 0
                Layout.preferredWidth: root.sidebarWidth
                Layout.preferredHeight: root.headerHeight

            }

            Rectangle {
                id: verticalDivider
                Layout.row: 0
                Layout.column: 1
                Layout.preferredWidth: 1
                Layout.rowSpan: 3
                Layout.fillHeight: true
                color: "white"
                opacity: 0.2
            }

            Item {
                id: contentHeader
                Layout.row: 0
                Layout.column: 2
                Layout.fillWidth: true
                Layout.preferredHeight: root.headerHeight

            }

            Rectangle {
                id: horizontalDivider
                Layout.row: 1
                Layout.column: 0
                Layout.columnSpan: 3
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "white"
                opacity: 0.2
            }

            Item {
                id: sidebarBody
                Layout.row: 2
                Layout.column: 0
                Layout.preferredWidth: root.sidebarWidth
                Layout.fillHeight: true

            }

            Item {
                id: contentBody
                Layout.row: 2
                Layout.column: 2
                Layout.fillWidth: true
                Layout.fillHeight: true

            }
        }
    }
}
