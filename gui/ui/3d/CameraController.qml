import QtQuick
import QtQuick3D

Item {
    id: root

    enum Axis {
        X,
        Y,
        Z
    }

    required property PerspectiveCamera perspective_camera
    property OrthographicCamera orthographic_camera

    readonly property Camera active_camera : use_orthographic ? orthographic_camera : perspective_camera

    property Node rotation_target
    property vector3d rotation_target_offset: Qt.vector3d(0,0,0)

    property bool use_orthographic: false
    property bool input_enabled: true
    property bool use_wasd: false

    function align_to_axis(axis, invert) {
        internal.current_controller.align_to_axis(axis, invert)
    }

    function build_align_vector(axis, invert) {
        var axis_setup = Qt.vector3d(0,0,0)

        switch (axis) {
        case CameraController.X:
            axis_setup = Qt.vector3d(1,0,0)
            break;
        case CameraController.Y:
            axis_setup = Qt.vector3d(0,1,0)
            break;
        case CameraController.Z:
            axis_setup = Qt.vector3d(0,0,1)
            break;
        default:
            return
        }

        if (invert) {
            axis_setup = axis_setup.times(-1.0)
        }

        return axis_setup
    }

    // internal functions

    onUse_orthographicChanged: {
        if (use_orthographic) {
            use_wasd = false
        }
    }

    focus: true

    Keys.onPressed: (event)=> { if (input_enabled && !event.isAutoRepeat) handleKeyPress(event) }
    Keys.onReleased: (event)=> { if (input_enabled && !event.isAutoRepeat) handleKeyRelease(event) }

    function handleKeyPress(event) {
        internal.current_controller.handleKeyPress(event)
    }

    function handleKeyRelease(event) {
        internal.current_controller.handleKeyRelease(event)
    }

    DragHandler {
        id: dragHandler
        target: null
        enabled: root.input_enabled
        //acceptedModifiers: Qt.NoModifier
        onCentroidChanged: {
            root.mouseMoved(Qt.vector2d(centroid.position.x, centroid.position.y), false);
        }

        onActiveChanged: {
            if (active)
                root.mousePressed(Qt.vector2d(centroid.position.x, centroid.position.y));
            else
                root.mouseReleased(Qt.vector2d(centroid.position.x, centroid.position.y));
        }
    }

    // ============================================================
    // INTERNAL STATE
    // ============================================================

    QtObject {
        id: internal

        // Needed to guard moves against press and release ordering
        property bool is_mouse_down: false

        property vector2d last_pos: Qt.vector2d(0, 0)
        property vector2d mouse_delta_pos: Qt.vector2d(0, 0)

        property QtObject current_controller : root.use_wasd
                                               ? wasd_control
                                               :
                                                 orbit_control
    }



    function mousePressed(coord) {
        forceActiveFocus()
        internal.is_mouse_down = true
        internal.mouse_delta_pos = Qt.vector2d(0,0)
        internal.last_pos = coord

        if (!root.use_wasd) {
            orbit_control.initialize_from_camera()
        }
    }

    function mouseReleased(coord) {
        //console.log("RELEASE")
        internal.is_mouse_down = false
        internal.mouse_delta_pos = Qt.vector2d(0,0)
        internal.last_pos = coord
    }

    function mouseMoved(coord) {
        //console.log("MOVED")
        if (!internal.is_mouse_down) return
        internal.mouse_delta_pos = coord.minus(internal.last_pos)
        internal.last_pos = coord
        internal.current_controller.apply_mouse_delta(coord)
        internal.mouse_delta_pos = Qt.vector2d(0,0)
    }

    // DragHandler {
    //     id: ctrlDragHandler
    //     target: null
    //     enabled: root.mouseEnabled && root.panEnabled
    //     acceptedButtons: root.acceptedButtons
    //     acceptedModifiers: Qt.ControlModifier
    //     onCentroidChanged: {
    //         root.panEvent(Qt.vector2d(centroid.position.x, centroid.position.y));
    //     }

    //     onActiveChanged: {
    //         if (active)
    //             root.startPan(Qt.vector2d(centroid.position.x, centroid.position.y));
    //         else
    //             root.endPan();
    //     }
    // }

    // PinchHandler {
    //     id: pinchHandler
    //     target: null
    //     enabled: root.mouseEnabled
    //     onScaleChanged: (delta) => {
    //                         root.camera.z = root.camera.z * (1 / delta)
    //                     }
    // }

    // TapHandler {
    //     acceptedButtons: root.acceptedButtons
    //     onTapped: root.forceActiveFocus() // qmllint disable signal-handler-parameters
    // }

    WheelHandler {
        id: wheel_handler

        orientation: Qt.Vertical
        target: null
        enabled: root.input_enabled

        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad

        onWheel: function(event) {
            if (root.use_wasd) {
                wasd_control.speed_multiplier *=
                        Math.exp(wasd_control.scroll_factor * event.angleDelta.y)

                wasd_control.speed_multiplier = Math.max(
                            Math.min(wasd_control.speed_multiplier, 100000),
                            0.000001)
            } else {
                orbit_control.apply_wheel_delta(event.angleDelta.y)
            }
        }
    }

    FrameAnimation {
        running: true
        onTriggered: {
            internal.current_controller.processInput(frameTime)
        }
    }

    ParallelAnimation {
        id: wasd_camera_animation

        Vector3dAnimation {
            id: camera_move_animation
            duration: 250

            target: root.active_camera
            property: "position"

            easing: Easing.InOutCubic

        }

        QuaternionAnimation {
            id: camera_rotation_animation
            duration: 250

            target: root.active_camera
            property: "rotation"

            easing: Easing.InOutCubic
        }

        onFinished: {
            console.log("Animation done")
        }
    }

    // ============================================================
    // WASD CONTROLLER
    // ============================================================

    QtObject {
        id: wasd_control

        property real sensitivity: 0.2
        property real walk_speed: 5.0
        property real run_speed: 15.0
        property real scroll_factor: 0.04879016 / 4.0
        //property real friction: 40.0
        property real friction: 1.5
        property real mouse_sensitivity_deg: 0.3

        property bool is_animating: false


        property bool initialized: false
        property real speed_multiplier: 1.0
        property vector3d velocity: Qt.vector3d(0,0,0)

        property vector3d kb_state: Qt.vector3d(0,0,0)
        property bool kb_shift: false


        function negate(vector) {
            return Qt.vector3d(-vector.x, -vector.y, -vector.z)
        }

        function processInput(frameDelta) {
            if (is_animating) {
                return;
            }

            //console.log(velocity)
            var is_moving = !kb_state.fuzzyEquals(Qt.vector3d(0,0,0))

            if (is_moving) {
                let speed = (kb_shift ? run_speed : walk_speed) * speed_multiplier

                velocity = kb_state.normalized().times(speed)
            } else {
                velocity = velocity.times(1.0 / friction)

                if (velocity.length() < 1e-6) {
                    velocity = Qt.vector3d(0,0,0)
                }
            }

            is_moving = !velocity.fuzzyEquals(Qt.vector3d(0,0,0))

            if (is_moving) {
                let forward = root.active_camera.forward
                let right = root.active_camera.right

                forward = Qt.vector3d(forward.x, forward.y, forward.z)
                right = Qt.vector3d(right.x, right.y, right.z)

                let mlt = velocity.times(frameDelta)

                var xp = right.times(mlt.x)
                var yp = Qt.vector3d(0,1,0).times(mlt.y)
                var zp = forward.times(mlt.z)

                //console.log("X", xp, yp, zp)

                let delta = xp.plus(yp).plus(zp)

                let current_pos = root.active_camera.position

                let new_pos = Qt.vector3d(current_pos.x, current_pos.y, current_pos.z).plus(delta)

                root.active_camera.position = Qt.vector3d(new_pos.x, new_pos.y, new_pos.z)
            }

            //console.log(root.active_camera.position, kb_state)

        }

        function apply_mouse_delta(coord) {
            var rotationVector = root.active_camera.eulerRotation;
            let pitch = rotationVector.x
            let yaw = rotationVector.y

            pitch = (pitch - internal.mouse_delta_pos.y * mouse_sensitivity_deg * sensitivity)
            pitch = Math.max(Math.min(pitch, 89), - 89)

            yaw -= internal.mouse_delta_pos.x * mouse_sensitivity_deg * sensitivity

            root.active_camera.eulerRotation.x = pitch
            root.active_camera.eulerRotation.y = yaw
        }

        function align_to_axis(axis, invert) {
            var axis_setup = root.build_align_vector(axis, invert)

            var rotation = Quaternion.lookAt(
                        Qt.vector3d(0, 0, 0),
                        axis_setup)

            camera_move_animation.from = root.active_camera.position
            camera_move_animation.to = root.active_camera.position

            camera_rotation_animation.from = root.active_camera.rotation
            camera_rotation_animation.to = rotation

            is_animating = true
            wasd_camera_animation.start()
        }

        function handleKeyPress(event) {
            //console.log("Press", event)
            switch (event.key) {
            case Qt.Key_W:
                kb_state.z = 1.0
                break;
            case Qt.Key_S:
                kb_state.z = -1.0
                break;
            case Qt.Key_A:
                kb_state.x = -1.0
                break;
            case Qt.Key_D:
                kb_state.x = 1.0
                break;
            case Qt.Key_Q:
                kb_state.y = -1.0
                break;
            case Qt.Key_E:
                kb_state.y = 1.0
                break;
            case Qt.Key_Shift:
                kb_shift = true
                break;
            }
        }

        function handleKeyRelease(event) {
            //console.log("Release", event)
            switch (event.key) {
            case Qt.Key_W:
            case Qt.Key_S:
                kb_state.z = 0.0
                break;
            case Qt.Key_A:
            case Qt.Key_D:
                kb_state.x = 0.0
                break;
            case Qt.Key_Q:
            case Qt.Key_E:
                kb_state.y = 0.0
                break;
            case Qt.Key_Shift:
                kb_shift = false
                break;
            }
        }
    }

    // ============================================================
    // ORBIT CONTROLLER
    // ============================================================

    QtObject {
        id: orbit_control

        property real sensitivity: 0.2
        property real mouse_sensitivity_deg: 0.6

        property real yaw_deg: 0
        property real pitch_deg: 0

        property real min_pitch_deg: -89
        property real max_pitch_deg: 89

        property real zoom_factor: 0.0005

        property real min_distance: 0.01
        property real max_distance: 1000000.0

        property bool is_animating: false

        property real animation_duration: 0.25
        property real animation_time: 0.0

        property real animation_from_yaw_deg: 0
        property real animation_from_pitch_deg: 0
        property real animation_from_distance: 1

        property real animation_to_yaw_deg: 0
        property real animation_to_pitch_deg: 0
        property real animation_to_distance: 1

        property vector3d rotation_point: {
            let base = root.rotation_target
                ? root.rotation_target.scenePosition
                : Qt.vector3d(0, 0, 0)

            return base.plus(root.rotation_target_offset)
        }

        function lerp(a, b, t) {
            return a + (b - a) * t
        }

        function shortest_angle_delta_deg(from_deg, to_deg) {
            var delta = to_deg - from_deg

            while (delta > 180.0)
                delta -= 360.0

            while (delta < -180.0)
                delta += 360.0

            return delta
        }

        function lerp_angle_deg(from_deg, to_deg, t) {
            return from_deg + shortest_angle_delta_deg(from_deg, to_deg) * t
        }

        function deg_to_rad(deg) {
            return deg * Math.PI / 180.0
        }

        function clamp(value, min_value, max_value) {
            return Math.max(min_value, Math.min(max_value, value))
        }

        function processInput(frameDelta) {
            if (!is_animating)
                return

            animation_time += frameDelta

            var t = animation_time / animation_duration
            t = clamp(t, 0.0, 1.0)

            // Smoothstep easing.
            var eased = t * t * (3.0 - 2.0 * t)

            yaw_deg = lerp_angle_deg(animation_from_yaw_deg,
                                     animation_to_yaw_deg,
                                     eased)

            pitch_deg = lerp(animation_from_pitch_deg,
                             animation_to_pitch_deg,
                             eased)

            var distance = lerp(animation_from_distance,
                                animation_to_distance,
                                eased)

            apply_orbit_transform(distance)

            if (t >= 1.0) {
                is_animating = false
                yaw_deg = animation_to_yaw_deg
                pitch_deg = animation_to_pitch_deg
                apply_orbit_transform(animation_to_distance)
            }
        }

        function initialize_from_camera() {
            var cam = root.active_camera
            var target = rotation_point

            var offset = cam.position.minus(target)

            var horizontal_distance = Math.sqrt(
                        offset.x * offset.x +
                        offset.z * offset.z)

            yaw_deg = Math.atan2(offset.x, offset.z) * 180.0 / Math.PI
            pitch_deg = Math.atan2(offset.y, horizontal_distance) * 180.0 / Math.PI

            pitch_deg = clamp(pitch_deg, min_pitch_deg, max_pitch_deg)
        }

        function orbit_rotation() {
            var yaw_q = Quaternion.fromAxisAndAngle(
                        Qt.vector3d(0,1,0),
                        yaw_deg)

            var right_axis = yaw_q.times(Qt.vector3d(1, 0, 0)).normalized()

            // Important: negate pitch here.
            // initialize_from_camera() reads positive pitch as camera above target.
            // Qt rotation around +X needs negative angle to recreate that offset.
            var pitch_q = Quaternion.fromAxisAndAngle(
                        right_axis,
                        -pitch_deg)

            return pitch_q.times(yaw_q).normalized()
        }

        function apply_orbit_transform(distance) {
            var cam = root.active_camera
            var target = rotation_point

            var q = orbit_rotation()

            var local_orbit_offset = Qt.vector3d(0, 0, distance)
            var new_offset = q.times(local_orbit_offset)

            cam.position = target.plus(new_offset)
            cam.rotation = q
        }


        function apply_mouse_delta(coord) {
            var cam = root.active_camera
            var target = rotation_point

            var offset = cam.position.minus(target)
            var distance = offset.length()

            if (distance < 0.000001)
                distance = 0.000001

            yaw_deg -= internal.mouse_delta_pos.x *
                    mouse_sensitivity_deg *
                    sensitivity

            pitch_deg += internal.mouse_delta_pos.y *
                    mouse_sensitivity_deg *
                    sensitivity

            pitch_deg = clamp(pitch_deg, min_pitch_deg, max_pitch_deg)

            apply_orbit_transform(distance)
        }


        function apply_wheel_delta(delta_y) {
            var cam = root.active_camera
            var target = rotation_point

            initialize_from_camera()

            var offset = cam.position.minus(target)
            var distance = offset.length()

            if (distance < 0.000001)
                return

            var zoom_scale = Math.exp(-delta_y * zoom_factor)
            var new_distance = clamp(distance * zoom_scale,
                                     min_distance,
                                     max_distance)

            apply_orbit_transform(new_distance)
        }

        function yaw_pitch_from_offset(offset) {
            var horizontal_distance = Math.sqrt(
                        offset.x * offset.x +
                        offset.z * offset.z)

            var result = {
                yaw: Math.atan2(offset.x, offset.z) * 180.0 / Math.PI,
                pitch: Math.atan2(offset.y, horizontal_distance) * 180.0 / Math.PI
            }

            return result
        }

        function align_to_axis(axis, invert) {
            var cam = root.active_camera
            var target = rotation_point

            initialize_from_camera()

            var current_offset = cam.position.minus(target)
            var current_distance = current_offset.length()

            if (current_distance < 0.000001)
                current_distance = 1.0

            var axis_offset = build_align_vector(axis, invert)
            var desired_offset = axis_offset.times(current_distance)

            var angles = yaw_pitch_from_offset(desired_offset)

            var target_pitch = clamp(angles.pitch,
                                     min_pitch_deg,
                                     max_pitch_deg)

            start_orbit_animation(angles.yaw,
                                  target_pitch,
                                  current_distance)
        }


        function start_orbit_animation(to_yaw_deg, to_pitch_deg, to_distance) {
            var cam = root.active_camera
            var target = rotation_point

            initialize_from_camera()

            var offset = cam.position.minus(target)
            var distance = offset.length()

            if (distance < 0.000001)
                distance = 1.0

            animation_from_yaw_deg = yaw_deg
            animation_from_pitch_deg = pitch_deg
            animation_from_distance = distance

            animation_to_yaw_deg = to_yaw_deg
            animation_to_pitch_deg = to_pitch_deg
            animation_to_distance = to_distance

            animation_time = 0.0
            is_animating = true
        }

        function handleKeyPress(event) {
            //console.log("Press", event)
            // switch (event.key) {
            // case Qt.Key_W:
            //     kb_state.z = 1.0
            //     break;
            // case Qt.Key_S:
            //     kb_state.z = -1.0
            //     break;
            // case Qt.Key_A:
            //     kb_state.x = -1.0
            //     break;
            // case Qt.Key_D:
            //     kb_state.x = 1.0
            //     break;
            // case Qt.Key_Q:
            //     kb_state.y = -1.0
            //     break;
            // case Qt.Key_E:
            //     kb_state.y = 1.0
            //     break;
            // case Qt.Key_Shift:
            //     kb_shift = true
            //     break;
            // }
        }

        function handleKeyRelease(event) {
            //console.log("Release", event)
            // switch (event.key) {
            // case Qt.Key_W:
            // case Qt.Key_S:
            //     kb_state.z = 0.0
            //     break;
            // case Qt.Key_A:
            // case Qt.Key_D:
            //     kb_state.x = 0.0
            //     break;
            // case Qt.Key_Q:
            // case Qt.Key_E:
            //     kb_state.y = 0.0
            //     break;
            // case Qt.Key_Shift:
            //     kb_shift = false
            //     break;
            // }
        }
    }
}
