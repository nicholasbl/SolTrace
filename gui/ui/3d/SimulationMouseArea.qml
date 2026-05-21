import QtQuick

MouseArea {
    id: simMouseArea

    required property var view
    required property var controller
    required property var gizmoOverlay
    required property var geometryInstanceContextMenu
    required property var root

    readonly property var axisDirs: [
        Qt.vector3d(1, 0, 0),
        Qt.vector3d(0, 1, 0),
        Qt.vector3d(0, 0, 1)
    ]

    function toScene(p) {
        return Qt.vector3d(p.x, p.z, -p.y)
    }

    function projectMouseToAxis(dx, dy, axisDir) {
        var ie = App.layout.instance_edit
        if (!ie) return 0
        var worldPos = ie.position
        var cam = controller.active_camera
        var scenePos = toScene(worldPos)
        var sceneTip = toScene(Qt.vector3d(
            worldPos.x + axisDir.x * 100,
            worldPos.y + axisDir.y * 100,
            worldPos.z + axisDir.z * 100
        ))
        var screenOrigin = cam.mapToViewport(scenePos)
        var screenTip = cam.mapToViewport(sceneTip)
        var screenDirX = (screenTip.x - screenOrigin.x) * view.width
        var screenDirY = (screenTip.y - screenOrigin.y) * view.height
        var len = Math.sqrt(screenDirX * screenDirX + screenDirY * screenDirY)
        if (len < 0.001) return 0
        screenDirX /= len
        screenDirY /= len
        return (dx * screenDirX + dy * screenDirY) * 0.5
    }

    function screenAngleToObject(mx, my) {
        var ie = App.layout.instance_edit
        if (!ie) return 0
        var cam = controller.active_camera
        var sp = cam.mapToViewport(ie.position)
        var cx = sp.x * view.width
        var cy = sp.y * view.height
        return Math.atan2(my - cy, mx - cx) * (180.0 / Math.PI)
    }

    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onPressed: (mouse) => {
        if (App.view.simulation_content_view) return

        // Gizmo picking
        if (root.showGizmo && mouse.button === Qt.LeftButton) {
            var gizmoResult = gizmoOverlay.pick(mouse.x, mouse.y)
            if (gizmoResult.objectHit) {
                var name = gizmoResult.objectHit.objectName

                if (name === "mode_toggle") {
                    root.gizmoMode = (root.gizmoMode === 0) ? 1 : 0
                    root.activeAxis = -1
                    return
                }

                if (name.startsWith("axis_") && root.gizmoMode === 0) {
                    root.activeAxis = parseInt(name.split("_")[1])
                    root.isDragging = true
                    root.lastMousePos = Qt.point(mouse.x, mouse.y)
                    return
                }

                if (name.startsWith("plane_") && root.gizmoMode === 0) {
                    root.activeAxis = parseInt(name.split("_")[1]) + 3
                    root.isDragging = true
                    root.lastMousePos = Qt.point(mouse.x, mouse.y)
                    return
                }

                if (name.startsWith("rot_") && root.gizmoMode === 1) {
                    root.activeAxis = parseInt(name.split("_")[1])
                    root.isDragging = true
                    root.lastMousePos = Qt.point(mouse.x, mouse.y)
                    root.initialAngle = screenAngleToObject(mouse.x, mouse.y)
                    var ie = App.layout.instance_edit
                    if (ie) {
                        root.initialRotation = ie.orientation.toEulerAngles()
                    }
                    return
                }
            }
        }

        // Scene picking
        const result = view.pick(mouse.x, mouse.y)
        var object = result.objectHit
        if (!object) {
            root.activeAxis = -1
            return
        }

        if (!object.instancing && mouse.button === Qt.LeftButton) {
            // stub
        } else if (!object.instancing && mouse.button === Qt.RightButton) {
            // stub
        } else if (object.instancing && mouse.button === Qt.LeftButton) {
            const index = result.instanceIndex
            if (index < 0) return
            // Open editing view
            App.view.workflow_phase = 0
            App.view.left_panel.visible = true
            App.view.configure_section = 3
            App.view.editing_layout = true
            App.layout.edited_element = object.instancing.at(index)

        } else if (object.instancing && mouse.button === Qt.RightButton) {
            const index = result.instanceIndex
            if (index < 0) return

            if (geometryInstanceContextMenu.focused_group) {
                // stub
            }

            if (geometryInstanceContextMenu.visible) {
                geometryInstanceContextMenu.reopening = true
            }

            geometryInstanceContextMenu.focused_group = object
            geometryInstanceContextMenu.index = index
            geometryInstanceContextMenu.popup()
        }
    }

    onPositionChanged: (mouse) => {
        if (!root.isDragging || root.activeAxis < 0) return

        var ie = App.layout.instance_edit
        if (!ie) return

        var dx = mouse.x - root.lastMousePos.x
        var dy = mouse.y - root.lastMousePos.y

        // Rotation
        if (root.gizmoMode === 1 && root.activeAxis >= 0 && root.activeAxis < 3) {
            var currentAngle = screenAngleToObject(mouse.x, mouse.y)
            var deltaAngle = currentAngle - root.initialAngle

            while (deltaAngle > 180) deltaAngle -= 360
            while (deltaAngle < -180) deltaAngle += 360

            var camPos = controller.active_camera.position
            var objPos = ie.position
            var toCamera = Qt.vector3d(
                camPos.x - objPos.x,
                camPos.y - objPos.y,
                camPos.z - objPos.z
            )

            var sign = 1.0
            if (root.activeAxis === 0) sign = toCamera.x >= 0 ? -1.0 : 1.0
            else if (root.activeAxis === 1) sign = toCamera.y >= 0 ? -1.0 : 1.0
            else if (root.activeAxis === 2) sign = toCamera.z >= 0 ? -1.0 : 1.0

            var rx = root.initialRotation.x
            var ry = root.initialRotation.y
            var rz = root.initialRotation.z

            if (root.activeAxis === 0) rx += deltaAngle * sign
            else if (root.activeAxis === 1) ry += deltaAngle * sign
            else if (root.activeAxis === 2) rz += deltaAngle * sign

            ie.set_from_angles(Qt.vector3d(rx, ry, rz))
            return
        }

        // Translation
        root.lastMousePos = Qt.point(mouse.x, mouse.y)

        if (root.activeAxis < 3) {
            var dir = axisDirs[root.activeAxis]
            var amount = projectMouseToAxis(dx, dy, dir)
            ie.position = Qt.vector3d(
                ie.position.x + dir.x * amount,
                ie.position.y + dir.y * amount,
                ie.position.z + dir.z * amount
            )
        } else {
            var planeAxes = [[0, 1], [0, 2], [1, 2]]
            var axes = planeAxes[root.activeAxis - 3]
            var dir1 = axisDirs[axes[0]]
            var dir2 = axisDirs[axes[1]]
            var amount1 = projectMouseToAxis(dx, dy, dir1)
            var amount2 = projectMouseToAxis(dx, dy, dir2)
            ie.position = Qt.vector3d(
                ie.position.x + dir1.x * amount1 + dir2.x * amount2,
                ie.position.y + dir1.y * amount1 + dir2.y * amount2,
                ie.position.z + dir1.z * amount1 + dir2.z * amount2
            )
        }
    }

    onReleased: (mouse) => {
        if (mouse.button === Qt.LeftButton) {
            root.isDragging = false
            root.activeAxis = -1
        }
    }
}
