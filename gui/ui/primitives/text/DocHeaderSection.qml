import QtQuick 2.15
import SolTraceProto

Column {
    property alias header: header
    property alias body: body

    spacing: 0
    width: parent.width

    Header {
        id: header
        text: "Header"
        font.pointSize: 22
    }

    Item {
        width: 1
        height: 8
        visible: Settings.showDocumentation
    }

    Body {
        id: body
        width: parent.width
        text: Documentation.placeholderText1
        visible: Settings.showDocumentation
    }

    Item {
        width: 1
        height: 20
        visible: Settings.showDocumentation
    }
}
