import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import SolTrace

Rectangle {
    property var blur_source
    property int blurMax: 32
    property color glassColor: Qt.rgba(0, 0, 0, 0.25)

    id: root
    clip: true
    color: "transparent"

    function getPositionRelativeToWindow() {
        try {
            var pos = root.mapToItem(blur_source, 0, 0)
            return Qt.rect(pos.x, pos.y, root.width, root.height)
        } catch (e) {
            console.error("Error getting position:", e)
        }
        return Qt.rect(0, 0, 0, 0)
    }


    Rectangle {
        id: mask
        anchors.fill: parent
        layer.enabled: true
        radius: root.radius
    }


    ShaderEffectSource {
        id: effect_source
        anchors.fill: root
        sourceItem: root.blur_source
        sourceRect: getPositionRelativeToWindow()
        visible: false
        hideSource: false
        live: !root.isDestroying
    }


    MultiEffect {
        id: multiEffect
        anchors.fill: effect_source
        source: effect_source
        maskEnabled: true
        maskSource: mask
        autoPaddingEnabled: false
        blurEnabled: true
        blurMax: root.blurMax
        blurMultiplier: 1
        blur: 1
    }


    Rectangle {
        id: background
        color: root.glassColor
        border.width: 1
        border.color: Qt.rgba(0.9, 0.9, 0.9, 0.15)
        anchors.fill: parent
        radius: root.radius
    }
}
