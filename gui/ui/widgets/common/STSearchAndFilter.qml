import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

Rectangle {
    color: "transparent"
    border.color: Theme.lineColor
    border.width: 1
    width: 250
    height: 30
    radius: 10

    property alias search_text: search_fld.text
    //property alias search_text_role: search_fld.textRole
    //property alias search_suggestion_model : search_fld.suggestionModel

    RowLayout {
        anchors.fill: parent

        IconButton {
            source: "qrc:/icons/assets/icons/search.svg"
            label: ""
            iconSize: 18
            margins: 0
            Layout.leftMargin: 5
            Layout.alignment: Qt.AlignVCenter
        }

        // SearchField {
        //     id: search_fld
        //     Layout.fillHeight: true
        //     Layout.fillWidth: true
        // }

        GlassTextField {
            id: search_fld
            Layout.fillWidth: true
            Layout.preferredHeight: 25
            borderColor: "transparent"
            horizontalAlignment: TextInput.AlignLeft
            leftPadding: 5
            rightPadding: 5
            bottomPadding: 5
        }

        IconButton {
            id: filter_button
            Layout.fillHeight: true
            source: "qrc:/icons/assets/icons/filter.svg"
        }
    }



    // RowLayout {
    //     id: searchBoxContent
    //     spacing: 10
    //     anchors.left: filter_button.left
    //     anchors.right: parent.right
    //     anchors.verticalCenter: parent.verticalCenter
    //     anchors.leftMargin: 10
    //     anchors.rightMargin: 10

    //     // IconButton {
    //     //     source: "qrc:/icons/assets/icons/search.svg"
    //     //     label: ""
    //     //     iconSize: 18
    //     //     margins: 0
    //     //     Layout.alignment: Qt.AlignVCenter
    //     // }

    //     // GlassTextField {
    //     //     id: search_fld
    //     //     Layout.fillWidth: true
    //     //     Layout.preferredHeight: 25
    //     //     borderColor: "transparent"
    //     //     horizontalAlignment: TextInput.AlignLeft
    //     //     leftPadding: 5
    //     //     rightPadding: 5
    //     //     bottomPadding: 5
    //     // }
    // }
}
