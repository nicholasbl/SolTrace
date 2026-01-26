import QtQuick 2.15
import QtQuick.Controls
import SolTraceProto

MouseArea {
    id: root

    property string label: ""
    property bool checked: false
    property alias checkBox: checkBox

    // Renamed to avoid conflict with MouseArea's clicked signal
    signal checkBoxClicked()

    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    onClicked: {
        root.checkBoxClicked()
    }

    Row {
        id: row
        spacing: 5

        CheckBox {
            id: checkBox
            checked: root.checked
            enabled: false

            // Prevent CheckBox from handling clicks directly
            MouseArea {
                anchors.fill: parent
                onPressed: function(mouse) {
                    mouse.accepted = false  // Let parent MouseArea handle it
                }
            }
        }

        Label {
            text: root.label
            font.pointSize: Theme.controlLabelSize
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
