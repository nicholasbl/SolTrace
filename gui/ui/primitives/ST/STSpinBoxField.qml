import QtQuick 2.15
import QtQuick.Controls

import SolTrace

Column {
    id: root

    property string label: ""
    property real from: 0
    property real to: 100
    property real stepSize: 1
    property int decimals: 0
    property string suffix: ""

    spacing: 5

    STDoubleSpinBox {
        id: spinBox

        width: parent.width
        from: root.from
        to: root.to
        decimals: root.decimals
        stepSize: root.stepSize
    }

    Label {
        id: label

        text: root.label
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
    }
}
