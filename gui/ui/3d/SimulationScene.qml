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
                    App.view.workflow_phase = 0
                    App.view.left_panel.visible = true
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
                    App.view.workflow_phase = 0
                    App.view.left_panel.visible = true
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
                    App.view.workflow_phase = 0
                    App.view.left_panel.visible = true
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
                textureData: {
                    let elevation = App.sun.type == SunModule.Directional ? -Math.asin(App.sun.position.y) * 180 / Math.PI : App.sun.position.y
                    let index = 0

                    if (elevation > 20) index = 0
                    else if (elevation > 5) index = 1
                    else if (elevation > -5) index = 2
                    else index = 3

                    switch (index) {
                    case 0: return daySky
                    case 1: return lateAfternoonSky
                    case 2: return sunsetSky
                    case 3: return nightSky
                    }
                }
                mappingMode: Texture.LightProbe
            }

            ProceduralSkyTextureData {
                id: daySky
                sunColor: Qt.rgba(0, 0, 0, 0)
                skyTopColor: Qt.rgba(0.2, 0.35, 0.6, 1.0)
                skyHorizonColor: Qt.rgba(0.55, 0.65, 0.75, 1.0)
                groundHorizonColor: Qt.rgba(0.55, 0.65, 0.75, 1.0)
                groundBottomColor: Qt.rgba(0.275, 0.325, 0.375, 1.0)
            }

            ProceduralSkyTextureData {
                id: lateAfternoonSky
                sunColor: Qt.rgba(0, 0, 0, 0)
                skyTopColor: Qt.rgba(0.3, 0.3, 0.5, 1.0)
                skyHorizonColor: Qt.rgba(0.75, 0.6, 0.5, 1.0)
                groundHorizonColor: Qt.rgba(0.45, 0.45, 0.55, 1.0)
                groundBottomColor: Qt.rgba(0.2, 0.2, 0.3, 1.0)
            }

            ProceduralSkyTextureData {
                id: sunsetSky
                sunColor: Qt.rgba(0, 0, 0, 0)
                skyTopColor: Qt.rgba(0.15, 0.15, 0.35, 1.0)
                skyHorizonColor: Qt.rgba(0.9, 0.5, 0.3, 1.0)
                groundHorizonColor: Qt.rgba(0.5, 0.35, 0.3, 1.0)
                groundBottomColor: Qt.rgba(0.15, 0.1, 0.15, 1.0)
            }

            ProceduralSkyTextureData {
                id: nightSky
                sunColor: Qt.rgba(0, 0, 0, 0)
                skyTopColor: Qt.rgba(0.02, 0.02, 0.08, 1.0)
                skyHorizonColor: Qt.rgba(0.05, 0.05, 0.15, 1.0)
                groundHorizonColor: Qt.rgba(0.05, 0.05, 0.1, 1.0)
                groundBottomColor: Qt.rgba(0.02, 0.02, 0.05, 1.0)
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

            Model {
                id: pointSource

                source: "#Sphere"

                x: App.sun.position.x
                y: App.sun.position.y
                z: App.sun.position.z

                visible: App.sun.type == SunModule.PointSource


                materials: [
                    PrincipledMaterial {
                        metalness: 1
                        roughness: 0
                        baseColor: "yellow"
                    }
                ]
            }

            Node {
                id: sunDirectionRayGroup

                property vector3d sunDir: Qt.vector3d(App.sun.position.x,
                                                      App.sun.position.y,
                                                      App.sun.position.z)
                property int distance: 1000

                visible: App.sun.type == SunModule.Directional

                // Position the rays at the sun's location
                position: Qt.vector3d(sunDir.x * distance,
                                      sunDir.y * distance,
                                      sunDir.z * distance)

                // Orient so that local +Y points from sun toward origin.
                rotation: Quaternion.lookAt(
                    position,                    // source: where we are
                    Qt.vector3d(0, 0, 0),        // target: origin
                    Qt.vector3d(0, 1, 0),        // forward = +Y (cylinder's long axis!)
                    Qt.vector3d(0, 0, 1)         // up (any vector not parallel to forward)
                )

                // Ray arrows
                Repeater3D {
                    model: parent.generatePositions()
                    delegate: Node {
                        required property var modelData
                        position: modelData

                        // Shaft
                        Model {
                            source: "#Cylinder"
                            scale: Qt.vector3d(0.1, 5.0, 0.1)
                            materials: PrincipledMaterial {
                                metalness: 1; roughness: 0; baseColor: "yellow"
                            }
                        }

                        // Arrowhead
                        Model {
                            source: "#Cone"
                            position: Qt.vector3d(0, 250, 0)
                            scale: Qt.vector3d(0.3, 0.5, 0.3)
                            materials: PrincipledMaterial {
                                metalness: 1; roughness: 0; baseColor: "yellow"
                            }
                        }
                    }
                }

                function generatePositions() {
                    let rayPositions = []
                    let rayGridSpan = 2
                    let rayDistance = 100
                    let rayGridOffset = rayDistance * (rayGridSpan - 1) / 2
                    for (let i = 0; i < rayGridSpan; i++) {
                        for (let j = 0; j < rayGridSpan; j++) {
                            let x = i * rayDistance - rayGridOffset
                            let z = j * rayDistance - rayGridOffset
                            rayPositions.push(Qt.vector3d(x, 0, z))
                        }
                    }
                    return rayPositions
                }
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
