pragma Singleton
import QtQuick

QtObject {
    readonly property color textColor: "white"
    readonly property color labelColor: "white"
    readonly property color lineColor: Qt.rgba(1, 1, 1, 0.2)
    readonly property color buttonColor: Qt.rgba(1, 1, 1, 0.05)
    readonly property color buttonHoverColor: Qt.rgba(0, 0, 0, 0.05)
    readonly property color accentColor: Qt.rgba(1, 1, 1, 0.05)

    readonly property color glassColor: Qt.rgba(0, 0, 0, 0.25)
    readonly property color shadedGlassColor: Qt.rgba(0, 0, 0, 0.35)
    readonly property color shadedRowColor: Qt.rgba(0, 0, 0, 0.1)

    readonly property int controlRowLabelSize: 15
    readonly property int controlLabelSize: 14

    readonly property int textSize: 16
    readonly property int textSizeSmall: 14

    readonly property real listItemRectRadius: 6
    readonly property real comboPopupRectRadius: 6

}
