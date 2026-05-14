import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

Item {
    id: right_side

    readonly property var groupEditor: App.materials.group_edit
    readonly property var apertureEditor: groupEditor ? groupEditor.aperture_editor : null
    readonly property bool hasSelection: App.materials.current_material_name.length > 0

    function hasProp(obj, name) {
        return obj && typeof obj[name] !== "undefined"
    }

    function syncComboToValue(combo, value) {
        if (!combo || !combo.count || !value || value.length === 0) {
            return
        }
        for (let i = 0; i < combo.count; i++) {
            if (combo.textAt(i) === value) {
                combo.currentIndex = i
                return
            }
        }
    }

    function expectedSurfaceArgCount(kind) {
        const k = (kind || "").toLowerCase()
        if (k.includes("flat")) return 0
        if (k.includes("parab")) return 2
        if (k.includes("cone")) return 1
        if (k.includes("cyl")) return 1
        if (k.includes("spher")) return 1
        if (k.includes("torus")) return 2
        if (k.includes("hyper")) return 2
        return 1
    }

    function defaultSurfaceArguments(kind) {
        const k = (kind || "").toLowerCase()
        if (k.includes("flat")) return []
        if (k.includes("parab")) return [1.0, 1.0]
        if (k.includes("cone")) return [15.0]
        if (k.includes("cyl")) return [1.0]
        if (k.includes("spher")) return [0.05]
        if (k.includes("torus")) return [1.0, 0.25]
        if (k.includes("hyper")) return [1.0, 1.0]
        return [0.0]
    }

    function surfaceArgCount() {
        if (!groupEditor) return 0
        const current = groupEditor.surface_arguments || []
        return Math.max(expectedSurfaceArgCount(groupEditor.surface_kind), current.length)
    }

    function surfaceArgLabel(index) {
        const k = (groupEditor ? groupEditor.surface_kind : "").toLowerCase()
        if (k.includes("parab")) {
            return index === 0 ? "Focal Length X" : "Focal Length Y"
        }
        if (k.includes("cone")) return "Half Angle"
        if (k.includes("cyl")) return "Radius"
        if (k.includes("spher")) return "Vertex Curvature"
        if (k.includes("torus")) return index === 0 ? "Major Radius" : "Minor Radius"
        if (k.includes("hyper")) return index === 0 ? "Parameter A" : "Parameter B"
        return "Argument " + String(index + 1)
    }

    function surfaceArgValue(index) {
        if (!groupEditor) return 0
        const args = groupEditor.surface_arguments || []
        return (index >= 0 && index < args.length) ? args[index] : 0
    }

    function setSurfaceArg(index, value) {
        if (!groupEditor || index < 0) return
        const existing = groupEditor.surface_arguments || []
        const size = Math.max(existing.length, index + 1)
        const out = []
        for (let i = 0; i < size; i++) {
            out.push(i < existing.length ? existing[i] : 0)
        }
        out[index] = value
        groupEditor.surface_arguments = out
    }

    function setRectangleField(field, value) {
        if (!apertureEditor || !hasProp(apertureEditor, "rectangle")) return
        const r = apertureEditor.rectangle
        if (field === "x") apertureEditor.rectangle = Qt.rect(value, r.y, r.width, r.height)
        if (field === "y") apertureEditor.rectangle = Qt.rect(r.x, value, r.width, r.height)
        if (field === "width") apertureEditor.rectangle = Qt.rect(r.x, r.y, value, r.height)
        if (field === "height") apertureEditor.rectangle = Qt.rect(r.x, r.y, r.width, value)
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Q3D.View3D {
                id: preview_3d

                enabled: right_side.hasSelection

                anchors.fill: parent

                environment: Q3DH.ExtendedSceneEnvironment {
                    antialiasingMode: Q3D.SceneEnvironment.MSAA
                    antialiasingQuality: Q3D.SceneEnvironment.VeryHigh
                    probeExposure: 1.2
                    tonemapMode: Q3D.SceneEnvironment.TonemapModeAces
                    depthOfFieldEnabled: true
                    depthOfFieldBlurAmount: 4
                    depthOfFieldFocusDistance: camera.position.length()
                    depthOfFieldFocusRange: 100
                    backgroundMode: Q3D.SceneEnvironment.SkyBox
                    lightProbe: Q3D.Texture {
                        textureData: Q3DH.ProceduralSkyTextureData {
                            sunColor: Qt.rgba(0, 0, 0, 0)
                            skyTopColor: Qt.rgba(0.2, 0.35, 0.6, 1.0)
                            skyHorizonColor: Qt.rgba(0.55, 0.65, 0.75, 1.0)
                            groundHorizonColor: Qt.rgba(0.55, 0.65, 0.75, 1.0)
                            groundBottomColor: Qt.rgba(0.45, 0.55, 0.65, 1.0)
                        }
                        mappingMode: Q3D.Texture.LightProbe
                    }

                    Q3DH.InfiniteGrid {
                        visible: true
                        gridInterval: 1
                    }
                }

                property var geom_bb : App.materials.group_edit.surface_geometry.bounding_box
                property vector3d geom_center : geom_bb.max.minus(geom_bb.min).times(.5).plus(geom_bb.min)
                property real geom_bb_max_span : {
                    let span = geom_bb.max.minus(geom_bb.min)
                    return Math.max(span.x, Math.max(span.y, span.z))
                }
                property real geom_distance : Math.max(geom_bb_max_span / 2.0, 2)

                Q3D.Node {
                    id: camera_host

                    Q3D.PerspectiveCamera {
                        id: camera
                        position: Qt.vector3d(preview_3d.geom_distance, preview_3d.geom_distance, 0.0)

                        Component.onCompleted: {
                            lookAt(Qt.vector3d(0, 0, 0))
                        }

                        clipNear: .01
                        clipFar : 100

                        Behavior on position {
                            Vector3dAnimation {
                                duration: 2000
                            }
                        }
                    }

                    NumberAnimation on eulerRotation.y {
                        from: 0
                        to: 360
                        duration: 48000
                        loops: Animation.Infinite
                        running: true
                    }
                }

                Q3D.DirectionalLight {
                    eulerRotation.x: -45
                    eulerRotation.y: 45
                }

                Q3D.Node {
                    rotation: Q3D.Quaternion.fromEulerAngles(-90, 0, 0)
                    Q3D.Model {
                        id: surface_geometry
                        geometry: App.materials.group_edit.surface_geometry

                        materials : [
                            Q3D.PrincipledMaterial {
                                metalness: 1
                                roughness: 0.25
                                baseColor: "white"
                                cullMode: Q3D.Material.NoCulling
                            }
                        ]
                    }
                }
            }

            STLabel {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 6
                font.pointSize: 12
                text: "Geometry Preview"
            }

            Body {
                id: summary_copy
                anchors.fill: parent
                anchors.margins: 14
                visible: !right_side.hasSelection
                text: "Pick a material from the left sidebar to begin editing its optical properties."
                font.pointSize: 16
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                verticalAlignment: Qt.AlignVCenter
            }

            // TODO: seems to fire on everything
            STLabel {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                //visible: right_side.hasSelection && groupEditor.geometry_validation_status !== groupEditor.Ok
                visible: false
                id: warning_label
                text: "⚠"
                font.pointSize: 64
                color: "#ffff00"
            }
        }
    }
}
