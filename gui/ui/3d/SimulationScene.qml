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
                    let elevation = edit_node.elevation
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

        EditContentNode {
            id: edit_node
            visible: !App.view.simulation_content_view
        }

        SimulationResultNode {
            visible: App.view.simulation_content_view
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
           if (App.view.simulation_content_view) return

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
