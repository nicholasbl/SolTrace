import QtQuick 2.15
import SolTraceProto

Column {
    property alias header: header
    property alias body: body

    visible: UserSettings.showDocumentation
    spacing: 0
    width: parent.width

    Header {
        id: header
        text: "Header"
    }

    Item {
        width: 1
        height: 8
    }

    Body {
        id: body
        width: parent.width
        text: Documentation.placeholderText1
    }

    Item {
        width: 1
        height: 20
    }
}
