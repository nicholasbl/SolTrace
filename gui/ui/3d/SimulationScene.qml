import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick3D.AssetUtils
import QtQuick.Controls.Material

import SolTrace

Item {
    id: root

    function align_to_axis(axis, invert) {
        controller.align_to_axis(axis, invert)
    }

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
                    App.layout.edited_element = geometryInstanceContextMenu.focused_group.group_instances.at(geometryInstanceContextMenu.index)
                    App.view.configure_section = 3
                    App.view.editing_layout = true
                }
            }
        }
    }

    View3D {
        id: view
        anchors.fill: parent
        camera: controller.active_camera

        environment: SceneEnvironment {
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High

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
                    if (App.view.sim.blueprint_mode) return blueprintSky
                    let elevation = App.sun.type === SunModule.Directional ? sunDirectionRayGroup.z : pointSource.z
                    let index = 0

                    if (elevation > 30) return daySky
                    if (elevation > 10) return lateAfternoonSky
                    if (elevation > -10) return sunsetSky
                    return nightSky
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

            ProceduralSkyTextureData {
                id: blueprintSky
                sunColor: Qt.rgba(0, 0, 0, 0)
                skyTopColor: "#818182"
                skyHorizonColor: "#818182"
                groundHorizonColor: "#4d4d4d"
                groundBottomColor: "#4d4d4d"
            }

            InfiniteGrid {
                id: infiniteGrid
                visible: true
                gridInterval: 50
            }
        }

        PerspectiveCamera {
            id: main_perspective_camera
            z: 100
        }

        OrthographicCamera {
            id: main_ortho_camera
            z: 500
            clipNear: 0.01
        }

        DirectionalLight {
            eulerRotation.x: -45
            eulerRotation.y: 45
        }

        Node {
            id: world_node
            rotation: Quaternion.fromEulerAngles(-90, 0, 0)

            Repeater3D {
                visible: flux_repeater.count === 0
                model: App.layout.world_geometry_model

                delegate: Model {
                    id: geometry_model

                    required property var group_instances
                    required property var group_geometry
                    property bool is_focused: false

                    instancing: group_instances
                    geometry: group_geometry
                    pickable: true

                    materials: [
                        PrincipledMaterial {
                            metalness: App.view.sim.blueprint_mode ? 0 : 1
                            roughness: App.view.sim.blueprint_mode ? 1 : 0
                            baseColor: App.view.sim.geometry_color

                            lighting: App.view.sim.blueprint_mode ? PrincipledMaterial.NoLighting : PrincipledMaterial.FragmentLighting

                            cullMode: PrincipledMaterial.NoCulling
                        }
                    ]
                }
            }

            Connections {
                target: App.view.sim
                function onGeometry_color_changed() {
                    App.layout.world_geometry_model.set_all_color(App.view.sim.geometry_color)
                }
            }


            Model {
                visible: flux_repeater.count === 0
                geometry: AppData.intersections.ray_geometry

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
                scale: Qt.vector3d(App.view.sim.sun_viz_scale / 100, App.view.sim.sun_viz_scale / 100, App.view.sim.sun_viz_scale / 100)

                visible: App.sun.type === SunModule.PointSource && App.view.sim.sun_viz


                materials: [
                    PrincipledMaterial {
                        metalness: 1
                        roughness: 0
                        baseColor: App.view.sim.sun_color
                    }
                ]
            }

            Node {
                id: sunDirectionRayGroup

                property vector3d sunDir: Qt.vector3d(App.sun.position.x,
                                                      App.sun.position.y,
                                                      App.sun.position.z)
                property int distance: 1000

                visible: App.sun.type === SunModule.Directional && App.view.sim.sun_viz
                scale: Qt.vector3d(App.view.sim.sun_viz_scale / 100, App.view.sim.sun_viz_scale / 100, App.view.sim.sun_viz_scale / 100)

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
                            metalness: 0
                            roughness: 1
                            baseColor: App.view.sim.sun_color
                            }
                        }

                        // Arrowhead
                        Model {
                            source: "#Cone"
                            position: Qt.vector3d(0, 250, 0)
                            scale: Qt.vector3d(0.3, 0.5, 0.3)
                            materials: PrincipledMaterial {
                            metalness: 0
                            roughness: 1
                            baseColor: App.view.sim.sun_color
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
            Repeater3D {
                id: flux_repeater
                model: AppData.flux.flux_map_world_model

                delegate: Model {
                    required property var flux_position
                    required property var flux_rotation
                    required property var flux_geometry
                    required property var flux_texture_data
                    required property string flux_image_path

                    geometry: flux_geometry
                    //source: "#Cube"

                    position: flux_position
                    rotation: flux_rotation

                    Component.onCompleted: {
                        AppData.flux.current_image = flux_image_path
                    }

                    materials: [
                        PrincipledMaterial {
                            metalness: 1
                            roughness: 0
                            baseColor: "white"
                            cullMode: PrincipledMaterial.NoCulling
                            baseColorMap: Texture {
                                textureData: flux_texture_data
                            }
                        }
                    ]
                }
            }

            Model {
                id: iso_vol_mesh

                geometry: AppData.flux.ray_iso_volume

                materials: [
                    PrincipledMaterial {
                        metalness: 0
                        roughness: .5
                        baseColor: "white"
                        cullMode: PrincipledMaterial.NoCulling
                    }
                ]
            }

        }
    }

    CameraController {
        id: controller

        perspective_camera: main_perspective_camera
        orthographic_camera: main_ortho_camera

        use_wasd: App.view.sim.camera === SimulationViewState.WASD

        use_orthographic: App.view.sim.perspective === SimulationViewState.Orthographic

        anchors.fill: parent
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
