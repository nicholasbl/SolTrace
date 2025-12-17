import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Rectangle {
    property var source: null
    property int blurMax: 20
    property int radius: 10

    id: root
    clip: true
    color: "transparent"

    Timer {
        id: updateTimer
        interval: 16 // ~60fps
        repeat: true
        running: root.visible && root.source && typeof globalWindow !== "undefined" && globalWindow
        onTriggered: updateSourceRect()
    }

    Connections {
        target: typeof globalWindow !== "undefined" && globalWindow ? globalWindow : null

        // Handle window position changes
        function onXChanged() { updateSourceRect() }
        function onYChanged() { updateSourceRect() }

        // Handle window size changes
        function onWidthChanged() { updateSourceRect() }
        function onHeightChanged() { updateSourceRect() }

        // Handle window state changes
        function onVisibilityChanged() { updateSourceRect() }
    }

    onXChanged: updateSourceRect()
    onYChanged: updateSourceRect()
    onWidthChanged: updateSourceRect()
    onHeightChanged: updateSourceRect()
    onVisibleChanged: {
        if (visible) updateSourceRect()
    }

    function updateSourceRect() {
        if (root.source && root.visible) {
            effect_source.sourceRect = getPositionRelativeToWindow()
        }
    }

    function getPositionRelativeToWindow() {
        try {
            if (typeof globalWindow !== "undefined" && globalWindow && globalWindow.contentItem) {
                var pos = root.mapToItem(globalWindow.contentItem, 0, 0)
                return Qt.rect(pos.x, pos.y, root.width, root.height)
            } else {
                return sourceRect
            }
        } catch (e) {
            return Qt.rect(0, 0, root.width, root.height) // Fallback
        }
    }

    Rectangle {
        color: Qt.rgba(0, 0, 0, 0.15)
        border.width: 1
        border.color: Qt.rgba(0.9, 0.9, 0.9, 0.1)
        anchors.fill: parent
        radius: root.radius
        z: -1
    }

    ShaderEffectSource {
        id: effect_source
        anchors.fill: root
        sourceItem: root.source
        sourceRect: root.source ? getPositionRelativeToWindow() : Qt.rect(0, 0, 0, 0)
        visible: false
        z: -2
        hideSource: false
        live: true
    }

    Rectangle {
        id: mask
        anchors.fill: parent
        layer.enabled: true
        antialiasing: false
        radius: root.radius
        z: -3
    }

    MultiEffect {
        anchors.fill: effect_source
        source: effect_source
        maskEnabled: true
        maskSource: mask
        autoPaddingEnabled: false
        blurEnabled: true
        blurMax: root.blurMax
        blurMultiplier: 1
        blur: 1
        z: -2
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            root.source.sceneActive = false
        }
    }

    Component.onCompleted: {
        if (typeof globalWindow === "undefined" || !globalWindow) {
            console.error("globalWindow is not defined. Make sure to set it via context property:")
            console.error("engine.rootContext()->setContextProperty(\"globalWindow\", window);")
        } else {
            updateSourceRect()
        }
    }
}
