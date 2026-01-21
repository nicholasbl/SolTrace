import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import SolTraceProto

Rectangle {
    property var source: globalScene
    property int blurMax: 20
    property int radius: 10
    property color glassColor: Theme.glassColor

    id: root
    clip: true
    color: "transparent"
    focus: true

    property bool isDestroying: false

    Timer {
        id: updateTimer
        interval: 16
        repeat: true
        running: root.visible && root.source && globalWindow !== null && !root.isDestroying
        onTriggered: {
            if (root.isDestroying) {
                stop()
                return
            }
            updateSourceRect()
        }
    }

    Connections {
        id: windowConnections
        target: !root.isDestroying && globalWindow !== null ? globalWindow : null
        enabled: root.visible && !root.isDestroying

        function onXChanged() {
            if (!root.isDestroying) updateSourceRect()
        }
        function onYChanged() {
            if (!root.isDestroying) updateSourceRect()
        }
        function onWidthChanged() {
            if (!root.isDestroying) updateSourceRect()
        }
        function onHeightChanged() {
            if (!root.isDestroying) updateSourceRect()
        }
    }

    onXChanged: if (!isDestroying) updateSourceRect()
    onYChanged: if (!isDestroying) updateSourceRect()
    onWidthChanged: if (!isDestroying) updateSourceRect()
    onHeightChanged: if (!isDestroying) updateSourceRect()

    onVisibleChanged: {
        if (visible && !isDestroying) {
            updateSourceRect()
        }
    }

    // Make sure source changes are handled
    onSourceChanged: {
        if (isDestroying) return

        if (source) {
            updateSourceRect()
        } else {
            cleanup()
        }
    }

    function updateSourceRect() {
        if (isDestroying || !root.source || !root.visible) return

        try {
            if (globalWindow !== null) {
                effect_source.sourceRect = getPositionRelativeToWindow()
            }
        } catch (e) {
            console.error("Error updating source rect:", e)
        }
    }

    function getPositionRelativeToWindow() {
        if (isDestroying) return Qt.rect(0, 0, 0, 0)

        try {
            if (globalWindow !== null && globalWindow.contentItem) {
                var pos = root.mapToItem(globalWindow.contentItem, 0, 0)
                return Qt.rect(pos.x, pos.y, root.width, root.height)
            }
        } catch (e) {
            console.error("Error getting position:", e)
        }
        return Qt.rect(0, 0, 0, 0)
    }

    function cleanup() {
        if (isDestroying) return

        isDestroying = true

        // 1. Stop timer immediately
        updateTimer.running = false
        updateTimer.stop()

        // 2. Disable and disconnect connections
        windowConnections.enabled = false
        windowConnections.target = null

        // 3. Clean up MultiEffect first
        multiEffect.maskEnabled = false
        multiEffect.blurEnabled = false
        multiEffect.source = null
        multiEffect.maskSource = null

        // 4. Disable mask layer
        if (mask.layer) {
            mask.layer.enabled = false
        }

        // 5. Clean up ShaderEffectSource
        effect_source.live = false
        effect_source.hideSource = true
        effect_source.sourceItem = null

        // Small delay to ensure shader cleanup
        Qt.callLater(function() {
            if (effect_source) {
                effect_source.visible = false
            }
        })

        // 6. Clear source reference
        if (root.source) {
            if (root.source.sceneActive !== undefined) {
                root.source.sceneActive = false
            }
            root.source = null
        }
    }

    Rectangle {
        id: background
        color: root.glassColor
        border.width: 1
        border.color: Qt.rgba(0.9, 0.9, 0.9, 0.15)
        anchors.fill: parent
        radius: root.radius
        z: -1
        visible: !root.isDestroying
    }

    ShaderEffectSource {
        id: effect_source
        anchors.fill: root
        sourceItem: !root.isDestroying ? root.source : null
        sourceRect: Qt.rect(0, 0, 0, 0)
        visible: false
        z: -2
        hideSource: false
        live: !root.isDestroying

        // Prevent binding loops
        Component.onCompleted: {
            if (!root.isDestroying && root.source) {
                sourceRect = root.getPositionRelativeToWindow()
            }
        }
    }

    Rectangle {
        id: mask
        anchors.fill: parent
        layer.enabled: !root.isDestroying
        antialiasing: false
        radius: root.radius
        z: -3
        visible: !root.isDestroying
    }

    MultiEffect {
        id: multiEffect
        anchors.fill: effect_source
        source: !root.isDestroying ? effect_source : null
        maskEnabled: !root.isDestroying
        maskSource: !root.isDestroying ? mask : null
        autoPaddingEnabled: false
        blurEnabled: !root.isDestroying
        blurMax: root.blurMax
        blurMultiplier: 1
        blur: 1
        z: -2
        visible: !root.isDestroying
    }

    MouseArea {
        anchors.fill: parent
        enabled: !root.isDestroying

        onPressed: function(mouse) {
            console.log("Pressed")
            root.forceActiveFocus()

            if (root.source && !root.isDestroying) {
                root.source.sceneActive = false
            }
            mouse.accepted = true
        }
    }

    Component.onCompleted: {
        if (globalWindow !== null && root.source) {
            updateSourceRect()
        }
    }

    Component.onDestruction: {
        cleanup()
    }
}
