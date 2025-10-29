import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

import QtGraphs


import SolTraceProto

ApplicationWindow {
    id: window
    width: 1280
    height: 720
    visible: true

    Material.theme: Material.Dark
    Material.accent: Material.Purple

    Q3D.View3D {
        id: view
        anchors.fill: parent

        environment: Q3D.SceneEnvironment {
            backgroundMode: Q3D.SceneEnvironment.SkyBox
            lightProbe: Q3D.Texture {
                textureData: Q3DH.ProceduralSkyTextureData {
                }
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
                position: Qt.vector3d(0, 0, 50)
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
        }
    }

    TransparentPane {
        id: data_list_pane
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.margins: 10

        width: 200

        Material.elevation: 10
        Material.roundedScale: Material.MediumScale

        ColumnLayout {
            anchors.fill: parent
            Button {
                text: "Open..."

                onClicked: open_dialog.open()

                Layout.fillWidth: true
            }

            ListView {
                id: list_view
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: Backend.data_sets

                clip: true

                delegate: Pane {
                    id: data_set_delegate

                    width: ListView.view.width

                    height: 72

                    required property int index
                    required property string name
                    required property string provenance

                    Material.elevation: dsd_mouse.containsMouse ? 5 : 1

                    ColumnLayout {
                        id: dsd_layout
                        anchors.fill: parent
                        anchors.margins: 5

                        Label {
                            Layout.fillWidth: true
                            text: name

                            elide: Label.ElideRight
                        }

                        Label {
                            Layout.fillWidth: true
                            text: provenance

                            opacity: .5

                            elide: Label.ElideRight
                        }

                    }

                    MouseArea {
                        id: dsd_mouse
                        anchors.fill: dsd_layout

                        hoverEnabled: true

                        onClicked: {
                            list_view.model.select(data_set_delegate.index)
                        }
                    }
                }
            }
        }
    }

    TransparentPane {
        id: ray_source_pane
        anchors.left: data_list_pane.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.margins: 10
        visible: false

        width: 400

        Material.elevation: 10
        Material.roundedScale: Material.MediumScale

        ColumnLayout {
            anchors.fill: parent

            id: detail_pane

            property var current_set: Backend.data_sets.current_data
            property var ray_source : current_set.ray_source_model



            GroupBox {
                Layout.fillHeight: true
                Layout.fillWidth: true
                title: "Ray Source"

                ColumnLayout {
                    anchors.fill: parent

                    GridLayout {
                        Layout.fillWidth: true

                        columns: 2
                        Label {
                            text: "Position:"
                        }

                        Label {
                            Layout.fillWidth: true
                            text: detail_pane.ray_source.position.toString()
                            elide: Label.ElideRight
                        }

                        //

                        Label {
                            text: "Type:"
                        }

                        Label {
                            Layout.fillWidth: true
                            text: detail_pane.ray_source.shape
                            elide: Label.ElideRight
                        }

                        //

                        Label {
                            text: "Sigma:"
                        }

                        Label {
                            Layout.fillWidth: true
                            text: detail_pane.ray_source.sigma
                            elide: Label.ElideRight
                        }

                        //

                        Label {
                            text: "Half Width:"
                        }

                        Label {
                            Layout.fillWidth: true
                            text: detail_pane.ray_source.half_width
                            elide: Label.ElideRight
                        }

                        //

                        Label {
                            text: "Angles:"
                        }

                        Label {
                            Layout.fillWidth: true
                            text: detail_pane.ray_source.user_angle.toString()
                            elide: Label.ElideRight
                        }

                        //

                        Label {
                            text: "Intensity:"
                        }

                        Label {
                            Layout.fillWidth: true
                            text: detail_pane.ray_source.user_intensity.toString()
                            elide: Label.ElideRight

                            onTextChanged: {
                                console.log(detail_pane.ray_source.user_angle)
                                console.log(detail_pane.ray_source.user_intensity)
                            }
                        }
                    }

                    GraphsView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: width

                        axisX: ValueAxis {
                            titleText: "Angle (mrad)"
                            max: 15.0
                            tickInterval: 5
                        }
                        axisY: ValueAxis {
                            titleText: "Intensity"
                            max: 1.0
                            tickInterval: .25
                        }

                        panStyle: GraphsView.PanStyle.Drag
                        zoomStyle: GraphsView.ZoomStyle.Center

                        LineSeries {
                            id: sun_series
                            color: Material.color(Material.Blue)

                            GraphTransition {
                                GraphPointAnimation {
                                    duration: 1000;
                                    easingCurve.type: Easing.OutCubic
                                }
                            }

                            // Not amazing, but for now...
                            Connections {
                                target: detail_pane
                                function onCurrent_setChanged() {

                                    console.log("Updating sun chart...")

                                    let angles = detail_pane.ray_source.user_angle
                                    let intensities = detail_pane.ray_source.user_intensity

                                    let sun_p_count = Math.min(
                                        angles.length,
                                        intensities.length
                                    );

                                    let sun_points = []

                                    for (let sun_i = 0; sun_i < sun_p_count; sun_i++) {
                                        sun_points.push(
                                            Qt.point(
                                                angles[sun_i],
                                                intensities[sun_i]
                                            )
                                        )
                                    }

                                    sun_series.replace(sun_points)
                                }
                            }

                        }
                    }

}
            }
        }
    }
        TransparentPane {
            id: element_pane
            anchors.left: data_list_pane.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.margins: 10
            visible: true

            width: 400

            Material.elevation: 10
            Material.roundedScale: Material.MediumScale

            ColumnLayout {
                anchors.fill: parent

                property var current_set: Backend.data_sets.current_data
                property var ray_source : current_set.ray_source_model


                GroupBox {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    title: "Elements"

                    ColumnLayout {
                        anchors.fill: parent

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: detail_pane.current_set.element_model.surface_geometries
                        delegate: Row {
                            spacing: 1
                            height: 20

                            CheckBox {
                                checked: model.visible
                                onToggled: model.visible = checked   // two-way binding
                                anchors.verticalCenter: parent.verticalCenter
                                indicator.width: 14
                                indicator.height: 14
                            }

                            Text {
                                text: index + " " + model.label
                                color: "white"
                                anchors.verticalCenter: parent.verticalCenter
                            }


                        }

                        clip: true

                        ScrollIndicator.vertical: ScrollIndicator { }
                    }
}
            }


        }
    }

    FileDialog {
        id: open_dialog

        currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
        onAccepted: Backend.data_sets.start_load_file(selectedFile)
    }
}

/*##^##
Designer {
    D{i:0}D{i:1;cameraSpeed3d:25;cameraSpeed3dMultiplier:1}
}
##^##*/
