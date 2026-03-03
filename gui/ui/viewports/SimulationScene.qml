import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick3D.AssetUtils

FocusScope {
    id: root
    property bool sceneActive: false
    focus: true
    anchors.fill: parent

    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Escape && root.sceneActive) {
            root.sceneActive = false
            event.accepted = true
        }
    }

    View3D {
        anchors.fill: parent

        environment: SceneEnvironment {
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.VeryHigh

            temporalAAEnabled: true
            temporalAAStrength: 0.8

            aoStrength: 50
            aoDistance: 10
            aoSoftness: 75
            aoBias: 0.01
            aoSampleRate: 4

            probeExposure: 1.2

            tonemapMode: SceneEnvironment.TonemapModeAces

            backgroundMode: SceneEnvironment.SkyBox
            lightProbe: Texture {
                textureData: ProceduralSkyTextureData {
                    sunColor: Qt.rgba(0, 0, 0, 0)
                    skyTopColor: Qt.rgba(0.2, 0.35, 0.6, 1.0)
                    skyHorizonColor: Qt.rgba(0.55, 0.65, 0.75, 1.0)
                    groundHorizonColor: Qt.rgba(0.55, 0.65, 0.75, 1.0)
                    groundBottomColor: Qt.rgba(0.45, 0.55, 0.65, 1.0)

                }
                mappingMode: Texture.LightProbe
            }

            InfiniteGrid {
                id: infiniteGrid
                visible: true
                gridInterval: 50
            }
        }

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(500, 200, 500)
            eulerRotation: Qt.vector3d(10, 45, 0)
        }

        DirectionalLight {
            eulerRotation.x: -45
            eulerRotation.y: 45
        }



        Node {

            // for the moment, we are going to hang everything off here

            rotation: Quaternion.fromEulerAngles(-90,0,0)

            // Display scene geometry
            Repeater3D {
                model: Backend.world_geometry_model

                delegate: Model {
                    required property var group_instances;
                    required property var group_geometry;

                    instancing: group_instances
                    geometry: group_geometry

                    materials : [
                        PrincipledMaterial {
                            metalness: 1
                            roughness: 0
                            baseColor: "white"
                            //cullMode: Material.NoCulling
                            // TODO Remove after demo
                        }
                    ]
                }
            }

            // Model {
            //     geometry: Backend.results_backend.ray_geometry

            //     materials : [
            //         PrincipledMaterial {
            //             metalness: 0
            //             roughness: 1
            //             lighting: PrincipledMaterial.NoLighting
            //             baseColor: "white"
            //             baseColorMap: Texture {
            //                 source: "qrc:/images/assets/images/b_to_r_wide.png"
            //             }
            //         }
            //     ]
            // }
        }



    }

    SceneNavigator {
        id: mouseRotatorAndWASD
        anchors.fill: parent
        controlledObject: camera
        enabled: root.sceneActive
        keysEnabled: root.sceneActive
        mouseEnabled: root.sceneActive
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        preventStealing: false
        cursorShape: root.sceneActive ? Qt.SizeAllCursor : Qt.ArrowCursor
        drag.axis: Drag.YAxis

        onClicked: {
            root.sceneActive = true
            root.forceActiveFocus()
        }

        Connections {
            target: mouseArea
            function onWheel(wheel) {
                if (root.sceneActive) {
                    camera.fieldOfView += wheel.angleDelta.y * 0.04
                        * (camera.fieldOfView + wheel.angleDelta.y * 0.04 > 0.0)
                        * (camera.fieldOfView + wheel.angleDelta.y * 0.04 < 60.0)
                }
            }
        }

        PinchArea {
            id: pinchArea
            anchors.fill: parent
            enabled: root.sceneActive
            Connections {
                target: pinchArea
                function onPinchUpdated(pinch) {
                    if (pinch.previousScale - pinch.scale > 0.008 ||
                            pinch.previousScale - pinch.scale < -1 * 0.008) {
                        let velocity = (pinch.previousScale - pinch.scale) > 0 ? 1.25 : -1.25
                        camera.fieldOfView += velocity
                                * (camera.fieldOfView + velocity > 0.0)
                                * (camera.fieldOfView + velocity < 60.0)
                    }
                }
            }
        }
    }
}
