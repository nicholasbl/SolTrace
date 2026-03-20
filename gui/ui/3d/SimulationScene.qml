import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick3D.AssetUtils

import QtQuick.Controls

View3D {
    id: root

    property bool use_orthographic: false

    camera: use_orthographic ? ortho_camera : camera

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

    Node {
        id: origin

        eulerRotation: Qt.vector3d(-10, 45, 0)

        PerspectiveCamera {
            id: camera
            z: 100
        }

        OrthographicCamera {
            id: ortho_camera
            z: 500

            clipNear: 0.01
        }
    }


    DirectionalLight {
        eulerRotation.x: -45
        eulerRotation.y: 45
    }

    CustomOrbitController {
        anchors.fill: parent
        origin: origin
        camera: use_orthographic ? ortho_camera : camera
        automaticClipping: false
    }

    Node {

        // for the moment, we are going to hang everything off here

        rotation: Quaternion.fromEulerAngles(-90,0,0)

        // Display scene geometry
        Repeater3D {
            model: App.layout.world_geometry_model

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

    Button {
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        text: "Ortho"

        checked: root.use_orthographic

        checkable: true

        onClicked: root.use_orthographic = !root.use_orthographic
    }

}
