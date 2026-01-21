// SceneNavigator.qml
// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick3D

Item {
    id: root
    property Node controlledObject: cameraRoot

    property real speed: 1
    property real shiftSpeed: 3

    property real forwardSpeed: 5
    property real backSpeed: 5
    property real rightSpeed: 5
    property real leftSpeed: 5
    property real upSpeed: 5
    property real downSpeed: 5
    property real xSpeed: 0.1
    property real ySpeed: 0.1

    property bool xInvert: true
    property bool yInvert: false

    property bool mouseEnabled: true
    property bool keysEnabled: true

    readonly property bool inputsNeedProcessing: status.moveForward | status.moveBack
                                                 | status.moveLeft | status.moveRight
                                                 | status.moveUp | status.moveDown
                                                 | status.useMouse

    property alias acceptedButtons: dragHandler.acceptedButtons

    // World up vector for consistent vertical movement
    readonly property vector3d worldUp: Qt.vector3d(0, 1, 0)

    implicitWidth: parent.width
    implicitHeight: parent.height
    focus: keysEnabled

    DragHandler {
        id: dragHandler
        target: null
        enabled: mouseEnabled
        onCentroidChanged: {
            mouseMoved(Qt.vector2d(centroid.position.x, centroid.position.y));
        }

        onActiveChanged: {
            if (active)
                mousePressed(Qt.vector2d(centroid.position.x, centroid.position.y));
            else
                mouseReleased(Qt.vector2d(centroid.position.x, centroid.position.y));
        }
    }

    TapHandler {
        onTapped: root.forceActiveFocus()
    }

    Keys.onPressed: (event)=> { if (keysEnabled) handleKeyPress(event) }
    Keys.onReleased: (event)=> { if (keysEnabled) handleKeyRelease(event) }

    function mousePressed(newPos) {
        root.forceActiveFocus()
        status.currentPos = newPos
        status.lastPos = newPos
        status.useMouse = true;
    }

    function mouseReleased(newPos) {
        status.useMouse = false;
    }

    function mouseMoved(newPos) {
        status.currentPos = newPos;
    }

    function forwardPressed() {
        status.moveForward = true
        status.moveBack = false
    }

    function forwardReleased() {
        status.moveForward = false
    }

    function backPressed() {
        status.moveBack = true
        status.moveForward = false
    }

    function backReleased() {
        status.moveBack = false
    }

    function rightPressed() {
        status.moveRight = true
        status.moveLeft = false
    }

    function rightReleased() {
        status.moveRight = false
    }

    function leftPressed() {
        status.moveLeft = true
        status.moveRight = false
    }

    function leftReleased() {
        status.moveLeft = false
    }

    function upPressed() {
        status.moveUp = true
        status.moveDown = false
    }

    function upReleased() {
        status.moveUp = false
    }

    function downPressed() {
        status.moveDown = true
        status.moveUp = false
    }

    function downReleased() {
        status.moveDown = false
    }

    function shiftPressed() {
        status.shiftDown = true
    }

    function shiftReleased() {
        status.shiftDown = false
    }

    function handleKeyPress(event)
    {
        switch (event.key) {
        case Qt.Key_W:
        case Qt.Key_Up:
            forwardPressed();
            break;
        case Qt.Key_S:
        case Qt.Key_Down:
            backPressed();
            break;
        case Qt.Key_A:
        case Qt.Key_Left:
            leftPressed();
            break;
        case Qt.Key_D:
        case Qt.Key_Right:
            rightPressed();
            break;
        case Qt.Key_Q:
        case Qt.Key_PageUp:
            upPressed();
            break;
        case Qt.Key_E:
        case Qt.Key_PageDown:
            downPressed();
            break;
        case Qt.Key_Shift:
            shiftPressed();
            break;
        }
    }

    function handleKeyRelease(event)
    {
        switch (event.key) {
        case Qt.Key_W:
        case Qt.Key_Up:
            forwardReleased();
            break;
        case Qt.Key_S:
        case Qt.Key_Down:
            backReleased();
            break;
        case Qt.Key_A:
        case Qt.Key_Left:
            leftReleased();
            break;
        case Qt.Key_D:
        case Qt.Key_Right:
            rightReleased();
            break;
        case Qt.Key_Q:
        case Qt.Key_PageUp:
            upReleased();
            break;
        case Qt.Key_E:
        case Qt.Key_PageDown:
            downReleased();
            break;
        case Qt.Key_Shift:
            shiftReleased();
            break;
        }
    }

    Timer {
        id: updateTimer
        interval: 16
        repeat: true
        running: root.inputsNeedProcessing
        onTriggered: {
            processInputs();
        }
    }

    function processInputs()
    {
        if (root.inputsNeedProcessing)
            status.processInput();
    }

    // Helper function to negate a vector
    function negateVector(vector) {
        return Qt.vector3d(-vector.x, -vector.y, -vector.z)
    }

    // Function to update position based on a direction vector
    function updatePositionWithVector(direction, speed, position) {
        var actualSpeed = status.shiftDown ? speed * shiftSpeed : speed * root.speed;

        var velocity = Qt.vector3d(
            direction.x * actualSpeed,
            direction.y * actualSpeed,
            direction.z * actualSpeed
        );

        controlledObject.position = Qt.vector3d(
            position.x + velocity.x,
            position.y + velocity.y,
            position.z + velocity.z
        );
    }

    QtObject {
        id: status

        property bool moveForward: false
        property bool moveBack: false
        property bool moveLeft: false
        property bool moveRight: false
        property bool moveUp: false
        property bool moveDown: false
        property bool shiftDown: false
        property bool useMouse: false

        property vector2d lastPos: Qt.vector2d(0, 0)
        property vector2d currentPos: Qt.vector2d(0, 0)

        function processInput() {
            if (controlledObject == undefined)
                return;

            if (moveForward)
                updatePositionWithVector(controlledObject.forward, forwardSpeed, controlledObject.position);
            else if (moveBack)
                updatePositionWithVector(negateVector(controlledObject.forward), backSpeed, controlledObject.position);

            if (moveRight)
                updatePositionWithVector(controlledObject.right, rightSpeed, controlledObject.position);
            else if (moveLeft)
                updatePositionWithVector(negateVector(controlledObject.right), leftSpeed, controlledObject.position);

            // For up/down movement, always use world up vector
            if (moveDown)
                updatePositionWithVector(negateVector(root.worldUp), downSpeed, controlledObject.position);
            else if (moveUp)
                updatePositionWithVector(root.worldUp, upSpeed, controlledObject.position);

            if (useMouse) {
                // Get the delta
                var rotationVector = controlledObject.eulerRotation;
                var delta = Qt.vector2d(lastPos.x - currentPos.x,
                                        lastPos.y - currentPos.y);
                // rotate x
                var rotateX = -delta.x * xSpeed
                if (xInvert)
                    rotateX = -rotateX;
                    rotationVector.y += rotateX;

                // rotate y
                var rotateY = delta.y * ySpeed
                if (yInvert)
                    rotateY = -rotateY;
                if (rotationVector.x + rotateY <= 90.0 && rotationVector.x + rotateY >= -90.0)
                    rotationVector.x += rotateY;

                controlledObject.setEulerRotation(rotationVector);
                lastPos = currentPos;
            }
        }
    }
}
