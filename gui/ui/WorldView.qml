import QtQuick
import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH


import SolTraceProto


Q3D.View3D {
    id: view

    environment: Q3D.SceneEnvironment {
        backgroundMode: Q3D.SceneEnvironment.SkyBox
        lightProbe: Q3D.Texture {
            textureData: Q3DH.ProceduralSkyTextureData {
            }
        }

        Q3DH.InfiniteGrid {
            gridInterval: 10
        }
    }

    Q3DH.OrbitCameraController {
        anchors.fill: parent
        origin: cameraNode
        camera: camera
    }



    Q3D.Node {
        id: cameraNode
        Q3D.PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 10, 50)
        }
    }

    Q3D.DirectionalLight {
        eulerRotation.x: 250
        eulerRotation.y: -30
        brightness: 1.0
        ambientColor: "#7f7f7f"
    }

    Q3D.PrincipledMaterial {
        id: plane_material

        metalness: 0
        roughness: 0
    }

    Q3D.Model {
        position: Qt.vector3d(0.1, 0, 0)
        scale: Qt.vector3d(0.002, 0.0001, 0.0001)
        source: "#Cube"

        materials: [ Q3D.PrincipledMaterial {
                baseColor: "red"
            }
        ]

    }
    Q3D.Model {
        position: Qt.vector3d(0, 0.1, 0)
        scale: Qt.vector3d(0.0001, 0.002, 0.0001)
        source: "#Cube"

        materials: [ Q3D.PrincipledMaterial {
                baseColor: "green"
            }
        ]

    }
    Q3D.Model {
        position: Qt.vector3d(0, 0, 0.1)
        scale: Qt.vector3d(0.0001, 0.0001, 0.002)
        source: "#Cube"

        materials: [ Q3D.PrincipledMaterial {
                baseColor: "blue"
            }
        ]

    }

    Q3D.Node {
        id: elementsEnitity
        eulerRotation.x: -90
        Q3D.Repeater3D {
            model: detail_pane.current_set.element_model.surface_geometries
            delegate: Q3D.Model {
                geometry: model.geometry
                position: model.position
                rotation: model.rotation
                visible:  model.visible
                objectName: model.label

                materials: [
                    Q3D.PrincipledMaterial {
                        id: transmitterMaterial
                        baseColor: "#cccccc"
                        metalness: 0.5
                        roughness: 0.05
                        cullMode: Q3D.CustomMaterial.NoCulling
                    }
                ]
            }
        }

        // Model for showing output Ray Geometry
        Q3D.Model {
            geometry: detail_pane.current_set.element_model.ray_geometry
            materials: [
                Q3D.PrincipledMaterial {
                    id: ray_material
                    baseColor: "white"
                    metalness: 0.0
                    roughness: 1.0
                    lighting: Q3D.PrincipledMaterial.NoLighting

                    baseColorMap: Q3D.Texture {
                        source: "qrc:/assets/b_to_r_wide.png"
                    }
                }
            ]
        }

        Q3D.Model {
            source: "#Sphere"
            materials: [
                Q3D.PrincipledMaterial {
                    baseColor: "white"
                    metalness: 0.0
                    roughness: 1.0
                }
            ]

            scale: Qt.vector3d(0.01, 0.01, 0.01)

            instancing: detail_pane.current_set.element_model.ray_geometry.ray_volume
        }
    }
}


