import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick3D.AssetUtils
import QtQuick.Controls.Material

import SolTrace

Item {
    id: root

    property int activeAxis: -1
    property int gizmoMode: 0
    property bool showGizmo: App.view.editing_layout && App.layout.instance_edit
    property bool isDragging: false
    property point lastMousePos: Qt.point(0, 0)
    property real initialAngle: 0.0
    property vector3d initialRotation: Qt.vector3d(0, 0, 0)

    readonly property var axisDirs: [
        Qt.vector3d(1, 0, 0),
        Qt.vector3d(0, 1, 0),
        Qt.vector3d(0, 0, 1)
    ]

    function align_to_axis(axis, invert) {
        controller.align_to_axis(axis, invert)
    }

    Menu {
        id: geometryInstanceContextMenu
        property var focused_group: null
        property int index: -1
        property bool reopening: false

        enter: null
        exit: null

        onClosed: {
            if (reopening) return
            if (focused_group) {
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
                    if (App.view.sim.blueprint_mode) return App.theme.blueprintSky
                    let elevation = edit_node.elevation
                    if (elevation > 30) return App.theme.daySky
                    if (elevation > 10) return App.theme.lateAfternoonSky
                    if (elevation > -10) return App.theme.sunsetSky
                    return App.theme.nightSky
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
            id: mainPerspectiveCamera
            z: 100
        }

        OrthographicCamera {
            id: mainOrthoCamera
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

    GizmoOverlay {
        id: gizmoOverlay
        anchors.fill: parent
        sourceCamera: controller.active_camera
        gizmoPosition: App.layout.instance_edit ? App.layout.instance_edit.position : Qt.vector3d(0, 0, 0)
        activeAxis: root.activeAxis
        gizmoMode: root.gizmoMode
        enabled: root.showGizmo
    }

    CameraController {
        id: controller

        enabled: !root.isDragging

        perspective_camera: mainPerspectiveCamera
        orthographic_camera: mainOrthoCamera

        use_wasd: App.view.sim.camera === SimulationViewState.WASD
        use_orthographic: App.view.sim.perspective === SimulationViewState.Orthographic

        anchors.fill: parent

    }

    SimulationMouseArea {
        view: view
        controller: controller
        gizmoOverlay: gizmoOverlay
        geometryInstanceContextMenu: geometryInstanceContextMenu
        root: root
    }
}
