import QtQuick
import QtQuick3D
import QtQuick3D.Helpers

Item {
    property list<QtObject> axisModel: [
        QtObject {
            property color color: "red"
            property QtObject cylinder: QtObject {
                property vector3d position: Qt.vector3d(40, 0, 0)
                property vector3d rotation: Qt.vector3d(0, 0, -90)
            }
            property QtObject cone: QtObject {
                property vector3d position: Qt.vector3d(90, 0, 0)
                property vector3d rotation: Qt.vector3d(0, 0, -90)
            }
            property QtObject cuboid: QtObject {
                property vector3d position: Qt.vector3d(50, 0, 0)
                property vector3d rotation: Qt.vector3d(0, 0, 0)
            }
        },
        QtObject {
            property color color: "green"
            property QtObject cylinder: QtObject {
                property vector3d position: Qt.vector3d(0, 40, 0)
                property vector3d rotation: Qt.vector3d(0, 0, 0)
            }
            property QtObject cone: QtObject {
                property vector3d position: Qt.vector3d(0, 90, 0)
                property vector3d rotation: Qt.vector3d(0, 0, 0)
            }
            property QtObject cuboid: QtObject {
                property vector3d position: Qt.vector3d(0, 50, 0)
                property vector3d rotation: Qt.vector3d(0, 0, 0)
            }
        },
        QtObject {
            property color color: "blue"
            property QtObject cylinder: QtObject {
                property vector3d position: Qt.vector3d(0, 0, 40)
                property vector3d rotation: Qt.vector3d(90, 0, 0)
            }
            property QtObject cone: QtObject {
                property vector3d position: Qt.vector3d(0, 0, 90)
                property vector3d rotation: Qt.vector3d(90, 0, 0)
            }
            property QtObject cuboid: QtObject {
                property vector3d position: Qt.vector3d(0, 0, 50)
                property vector3d rotation: Qt.vector3d(0, 0, 0)
            }
        }
    ]

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
            backgroundMode: SceneEnvironment.Transparent
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
        }

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(150, 75, 150)
            eulerRotation: Qt.vector3d(-20, 45, 0)
        }

        DirectionalLight {
            eulerRotation.x: -45
            eulerRotation.y: 45
        }

        // Axis Repeater
        Repeater3D {
            model: axisModel

            Node {
                // Cylinder Shaft
                Model {
                    geometry: CylinderGeometry {
                        radius: 2
                        length: 80
                        rings: 1
                        segments: 16
                    }
                    materials: PrincipledMaterial {
                        baseColor: modelData.color
                        metalness: 0.5
                        roughness: 0.3
                    }
                    position: modelData.cylinder.position
                    eulerRotation: modelData.cylinder.rotation
                }

                // Cone Head
                Model {
                    geometry: ConeGeometry {
                        bottomRadius: 6
                        topRadius: 0
                        length: 20
                        rings: 1
                        segments: 16
                    }
                    materials: PrincipledMaterial {
                        baseColor: modelData.color
                        metalness: 0.5
                        roughness: 0.3
                    }
                    position: modelData.cone.position
                    eulerRotation: modelData.cone.rotation
                }

                // Cuboid
                Model {
                    geometry: CuboidGeometry {
                        xExtent: 12
                        yExtent: 12
                        zExtent: 12
                    }
                    materials: PrincipledMaterial {
                        baseColor: modelData.color
                        metalness: 0.5
                        roughness: 0
                    }
                    position: modelData.cuboid.position
                    eulerRotation: modelData.cuboid.rotation
                }
            }
        }

        // Center sphere
        Model {
            geometry: SphereGeometry {
                radius: 50
            }
            materials: PrincipledMaterial {
                baseColor: "#cccccc"
                metalness: 0.8
                roughness: 0.2
            }
            scale: Qt.vector3d(0.35, 0.35, 0.35)
        }
    }
}
