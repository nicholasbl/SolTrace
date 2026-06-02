import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick3D.AssetUtils

import QtQuick.Controls

import SolTrace

View3D {
    id: root

    environment: SceneEnvironment {
        antialiasingMode: SceneEnvironment.MSAA
        antialiasingQuality: SceneEnvironment.VeryHigh

        aoStrength: 20
        aoDistance: 10
        aoSoftness: 90
        aoBias: 0.01
        aoSampleRate: 4

        probeExposure: 0.8

        tonemapMode: SceneEnvironment.TonemapModeAces

        backgroundMode: SceneEnvironment.Transparent
        lightProbe: Texture {
            textureData: ProceduralSkyTextureData {
                sunColor: Qt.rgba(0, 0, 0, 0)
                skyTopColor: Qt.rgba(0.55, 0.58, 0.62, 1.0)
                skyHorizonColor: Qt.rgba(0.78, 0.80, 0.84, 1.0)
                groundHorizonColor: Qt.rgba(0.52, 0.54, 0.58, 1.0)
                groundBottomColor: Qt.rgba(0.30, 0.31, 0.34, 1.0)
            }
            mappingMode: Texture.LightProbe
        }

        InfiniteGrid {
            id: infiniteGrid
            visible: true
            gridInterval: 1
        }
    }

    PerspectiveCamera {
        id: camera
        position: Qt.vector3d(3,3,3)
        clipNear: 0.1
        clipFar: 100

        eulerRotation.x: -45
        eulerRotation.y: 45
    }


    DirectionalLight {
        ambientColor: "#50545A"
        eulerRotation: Qt.vector3d(-35, 35, 0)

        color: "#FFF2E0"
        castsShadow: true
        shadowFactor: 20
        shadowBias: 8

        brightness: 120
    }

    DirectionalLight {
        eulerRotation: Qt.vector3d(-20, -55, 0)

        color: "#DDEBFF"
        castsShadow: false

        brightness: 55
    }

    DirectionalLight {
        eulerRotation: Qt.vector3d(-15, 160, 0)

        color: "#FFFFFF"
        castsShadow: false

        brightness: 35
    }

    PointLight {
        position: Qt.vector3d(0, 3.5, 3)

        color: "#FFFFFF"
        castsShadow: false

        brightness: 120
        quadraticFade: 0.35
    }

    PointLight {
        position: Qt.vector3d(-3, -2, 2)

        color: "#E7F0FF"
        castsShadow: false

        brightness: 65
        quadraticFade: 0.45
    }

    CameraController {
        anchors.fill: parent
        perspective_camera: camera

        rotation_target: camera_target
    }

    Node {
        rotation: Quaternion.fromEulerAngles(-90,0,0)

        Model {
            geometry: App.materials.geometry_edit.surface_geometry

            castsShadows: true

            materials : [
                PrincipledMaterial {
                    metalness: 0.15
                    roughness: 0.35
                    baseColor: "white"
                }
            ]
        }

        Node {
            id: camera_target

            property var bb: App.materials.geometry_edit.surface_geometry.bounding_box

            property vector3d center: bb.max.minus(bb.min).times(0.5).plus(bb.min)

            position: center
        }

    }
}
