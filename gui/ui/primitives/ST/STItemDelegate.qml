import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

ItemDelegate {
    id: control
    width: ListView.view ? ListView.view.width : implicitWidth
    highlighted: ListView.isCurrent ? ListView.isCurrent : false

    hoverEnabled: true

    background: Rectangle {
        implicitHeight: 24
        implicitWidth: 100
        opacity: enabled ? 1 : 0.3
        color: parent.hovered
               ?
                   Material.highlightedRippleColor
                 :
                   parent.down
                   ?
                       Material.rippleColor
                     :
                       "transparent"
        radius: 14
    }
}