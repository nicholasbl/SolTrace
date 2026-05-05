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
        z: 5

        clipNear: 0.1
        clipFar: 100
    }


    DirectionalLight {
        eulerRotation.x: -45
        eulerRotation.y: 45
    }

    // TODO we need a proper camera controller with orbit
    // for perspective and ortho
    // AND zoom to control
    CameraController {
        anchors.fill: parent
        perspective_camera: camera
        orthographic_camera: camera
    }

    Node {
        rotation: Quaternion.fromEulerAngles(-90,0,0)

        Model {
            geometry: App.materials.geometry_edit.surface_geometry

            materials : [
                PrincipledMaterial {
                    metalness: 1
                    roughness: 0.1
                    baseColor: "white"
                    cullMode: Material.NoCulling
                }
            ]
        }

    }
}
