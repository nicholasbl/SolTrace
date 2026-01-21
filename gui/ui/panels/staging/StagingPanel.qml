import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto
Column {
    id: root
    anchors.fill: parent
    anchors.margins: 20
    anchors.rightMargin: 50
    spacing: 10
    DocHeaderSection {
        width: parent.width
        header.text: Documentation.staging.headers.staging
        body.text: Documentation.staging.staging
    }
}
