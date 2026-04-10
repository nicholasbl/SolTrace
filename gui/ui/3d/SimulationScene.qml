import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick3D.AssetUtils
import QtQuick.Controls.Material

import SolTrace

Item {
    id: root

    Menu {
        id: geometryInstanceContextMenu
        property var focused_group: null
        property int index: -1
        property bool reopening: false

        enter: null   // disable open animation
        exit: null    // disable close animation

        onClosed: {
            if (reopening) {
                // Skip case to prevent valid state from being reset
                return
            }

            if (focused_group) {
                // focused_group.is_focused = false
                focused_group = null
                index = -1
            }
        }

        MenuItem {
            text: "View/Edit Material"
            onClicked: {
                if (geometryInstanceContextMenu.focused_group) {
                    App.materials.current_material = geometryInstanceContextMenu.focused_group.group_instances.material_of(geometryInstanceContextMenu.index)
                    App.view.configure_section = 1
                    App.view.editing_material = true
                }
            }
        }
        MenuItem {
            text: "View/Edit Geometry"

            onClicked: {
                if (geometryInstanceContextMenu.focused_group) {
                    App.materials.current_geometry = geometryInstanceContextMenu.focused_group.group_instances.geometry_of(geometryInstanceContextMenu.index)
                    App.view.configure_section = 2
                    App.view.editing_geometry = true
                }
            }
        }
        MenuItem {
            text: "View/Edit Layout"
            onClicked: {
                if (geometryInstanceContextMenu.focused_group) {
                    App.layout.current_element = geometryInstanceContextMenu.focused_group.group_instances.at(geometryInstanceContextMenu.index)
                    App.view.configure_section = 3
                    App.view.editing_layout = true
                }
            }
        }
    }

    View3D {
        id: view
        anchors.fill: parent

        camera: App.view.perspective == ViewModule.Orthographic ? ortho_camera : camera

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
                    groundBottomColor: Qt.rgba(0.275, 0.325, 0.375, 1.0)
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

        Node {
            rotation: Quaternion.fromEulerAngles(-90, 0, 0)

            Repeater3D {
                model: App.layout.world_geometry_model

                delegate: Model {
                    required property var group_instances
                    required property var group_geometry
                    property bool is_focused: false

                    instancing: group_instances
                    geometry: group_geometry
                    pickable: true

                    materials: [
                        PrincipledMaterial {
                            metalness: 1
                            roughness: 0
                            baseColor: "white"
                            cullMode: PrincipledMaterial.NoCulling
                        }
                    ]
                }
            }

            Model {
                geometry: App.intersections.ray_geometry

                materials : [
                    PrincipledMaterial {
                        metalness: 0
                        roughness: 1
                        lighting: PrincipledMaterial.NoLighting
                        baseColor: "white"
                        baseColorMap: Texture {
                            source: "qrc:/assets/images/b_to_r_wide.png"
                        }
                    }
                ]
            }

        }
    }

    // I hate this stupid thing
    WasdController {
        mouseEnabled: App.view.camera == ViewModule.WASD
        keysEnabled: App.view.camera == ViewModule.WASD
        controlledObject: camera
    }

    CustomOrbitController {
        anchors.fill: parent
        origin: origin
        mouseEnabled: App.view.camera == ViewModule.Orbital
        panEnabled: App.view.camera == ViewModule.Orbital
        camera: App.view.perspective == ViewModule.Orthographic ? ortho_camera : camera
        automaticClipping: false
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: (mouse) => {
                       const result = view.pick(mouse.x, mouse.y)
                       var object = result.objectHit
                       if (!object) return

                       // Left-button picking with non-instanced geometry
                       if (!object.instancing && mouse.button === Qt.LeftButton) {
                           // stub
                       }
                       // Right-button picking with non-instanced geometry
                       else if (!object.instancing && mouse.button === Qt.RightButton) {
                           // stub
                       }
                       // Left-button picking with instanced geometry
                       else if (object.instancing && mouse.button === Qt.LeftButton) {
                           const index = result.instanceIndex
                           if (index < 0) return
                           object.instancing.toggle_selection(index)
                       }
                       // Right-button picking with instanced geometry
                       else if (object.instancing && mouse.button === Qt.RightButton) {
                           const index = result.instanceIndex
                           if (index < 0) return

                           if (geometryInstanceContextMenu.focused_group) {
                               // stub - release focus on previous state
                           }

                           if (geometryInstanceContextMenu.visible) {
                               geometryInstanceContextMenu.reopening = true
                           }

                           geometryInstanceContextMenu.focused_group = object
                           geometryInstanceContextMenu.index = index
                           geometryInstanceContextMenu.popup()
                       }
                   }
    }
}
