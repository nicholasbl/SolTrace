import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

ColumnLayout {
    id: root
    anchors.fill: parent
    anchors.margins: 20
    anchors.rightMargin: 50
    spacing: 10

    function comboIndex(options, value) {
        if (!options || options.length === 0) {
            return 0
        }

        const idx = options.indexOf(value)
        return idx >= 0 ? idx : 0
    }

    DocHeaderSection {
        width: parent.width
        header.text: Documentation.materials.headers.materials
        body.text: Documentation.materials.materials
    }
    Table {
        width: parent.width
        columnHeaders: ["Symbol", "Name", "Definition", "Units", "Range"]
        columnKeys: ["symbol", "name", "definition", "units", "range"]
        columnWidths: [150, 200, -1, 100, 150]
        firstColumnBold: true
        padding: 12
        columnsCentered: [true, true, false, true, true]
        columnsBolded: [true]
        visible: UserSettings.showDocumentation
        rowData: [
            {
                symbol: "ρ",
                name: "Reflectance",
                definition: Documentation.materials.reflectance,
                units: "—",
                range: "0-1"
            },
            {
                symbol: "τ",
                name: "Transmittance",
                definition: Documentation.materials.transmittance,
                units: "—",
                range: "0-1"
            },
            {
                symbol: "<em>n</em>",
                name: "Refractive Index",
                definition: Documentation.materials.refractiveIndex,
                units: "—",
                range: "1.0-3.0"
            },
            {
                symbol: "σ<sub>slope</sub>",
                name: "Slope Error",
                definition: Documentation.materials.slopeError,
                units: "mrad",
                range: "0-10"
            },
            {
                symbol: "σ<sub>spec</sub>",
                name: "Specularity Error",
                definition: Documentation.materials.specularityError,
                units: "mrad",
                range: "0-10"
            },
            {
                symbol: "ET",
                name: "Error Type",
                definition: Documentation.materials.errorType,
                units: "—",
                range: "Gaussian, Pillbox, Diffuse"
            },
            {
                symbol: "ρ(θ)",
                name: "Angular Reflectance",
                definition: Documentation.materials.angularReflectance,
                units: "—",
                range: "0-1"
            },
            {
                symbol: "τ(θ)",
                name: "Angular Transmittance",
                definition: Documentation.materials.angularTransmittance,
                units: "—",
                range: "0-1"
            }
        ]
    }
    Spacer { visible: UserSettings.showDocumentation}
    // SectionCallout {
    //     title: "Invalid Materials"
    //     OpticalMaterialsTable {}
    // }
    // Repeater {
    //     model: App.materials.materials_list
    //     SectionCallout {
    //         required property string name
    //         title: name
    //         OpticalMaterialsTable {}
    //     }
    // }

    Label {
        property string name : App.materials.current_material_name
        text: name.length === 0 ? "No Material Selected" : name
        font.pointSize: 17
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: width

        onHeightChanged: {
            console.log("RH CHANGED", height)
        }

        RowLayout {
            anchors.fill: parent

            Item {
                id: left_side
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.horizontalStretchFactor: 1

                onHeightChanged: {
                    console.log("LH CHANGED", height)
                }

                ScrollView {
                    anchors.fill: parent
                    clip: true
                    contentWidth: width

                    onContentHeightChanged: {
                        console.log("CONTENT H", contentHeight)
                    }

                    ColumnLayout {
                        id: material_editor
                        width: left_side.width
                        spacing: 16

                        readonly property bool hasSelection: App.materials.current_material_name.length > 0
                        readonly property var groupEditor: App.materials.group_edit
                        readonly property var frontEditor: groupEditor ? groupEditor.front_editor : null
                        readonly property var backEditor: groupEditor ? groupEditor.back_editor : null
                        readonly property var interactionOptions: groupEditor.interaction_type_model
                        readonly property var distributionOptions: groupEditor.distribution_type_model

                        onHasSelectionChanged: {
                            console.log("Material selection OK")
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            radius: 12
                            color: Qt.rgba(1, 1, 1, 0.04)
                            border.width: 1
                            border.color: Theme.lineColor
                            implicitHeight: summary_copy.implicitHeight + 28

                            Body {
                                id: summary_copy
                                anchors.fill: parent
                                anchors.margins: 14
                                text: material_editor.hasSelection
                                    ? "Edit front and back optical properties for the selected render group. Changes here update the live material preview on the right."
                                    : "Pick a material from the left sidebar to begin editing its optical properties."
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            radius: 14
                            color: Qt.rgba(1, 1, 1, 0.03)
                            border.width: 1
                            border.color: Theme.lineColor
                            enabled: material_editor.hasSelection
                            opacity: enabled ? 1.0 : 0.55
                            implicitHeight: front_column.implicitHeight + 28

                            ColumnLayout {
                                id: front_column
                                anchors.fill: parent
                                anchors.margins: 14
                                spacing: 12

                                RowLayout {
                                    Layout.fillWidth: true

                                    Label {
                                        text: "Front Side"
                                        font.pointSize: 15
                                        font.bold: true
                                    }

                                    Item {
                                        Layout.fillWidth: true
                                    }

                                    Button {
                                        text: "Absorber"
                                        enabled: material_editor.hasSelection && material_editor.frontEditor
                                        onClicked: material_editor.frontEditor.set_ideal_absorption()
                                    }

                                    Button {
                                        text: "Reflector"
                                        enabled: material_editor.hasSelection && material_editor.frontEditor
                                        onClicked: material_editor.frontEditor.set_ideal_reflection()
                                    }

                                    Button {
                                        text: "Transmitter"
                                        enabled: material_editor.hasSelection && material_editor.frontEditor
                                        onClicked: material_editor.frontEditor.set_ideal_transmission()
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 6

                                        Body {
                                            text: "Interaction"
                                            font.pointSize: Theme.controlRowLabelSize
                                        }

                                        GlassComboBox {
                                            id: front_interaction
                                            Layout.fillWidth: true
                                            model: material_editor.interactionOptions
                                            borderColor: Theme.lineColor
                                            enabled: material_editor.hasSelection && model.rowCount() > 0
                                            onActivated: {
                                                if (material_editor.frontEditor && currentText.length > 0) {
                                                    material_editor.frontEditor.interaction_type = currentText
                                                }
                                            }
                                        }

                                        // Binding {
                                        //     target: front_interaction
                                        //     property: "currentIndex"
                                        //     value: material_editor.interactionOptions.data() root.comboIndex(material_editor.interactionOptions, material_editor.frontEditor ? material_editor.frontEditor.interaction_type : "")
                                        // }
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 6

                                        Body {
                                            text: "Error Type"
                                            font.pointSize: Theme.controlRowLabelSize
                                        }

                                        GlassComboBox {
                                            id: front_distribution
                                            Layout.fillWidth: true
                                            model: material_editor.distributionOptions
                                            borderColor: Theme.lineColor
                                            enabled: material_editor.hasSelection && model.rowCount() > 0
                                            onActivated: {
                                                if (material_editor.frontEditor && currentText.length > 0) {
                                                    material_editor.frontEditor.error_distribution_type = currentText
                                                }
                                            }
                                        }

                                        // Binding {
                                        //     target: front_distribution
                                        //     property: "currentIndex"
                                        //     value: root.comboIndex(material_editor.distributionOptions, material_editor.frontEditor ? material_editor.frontEditor.error_distribution_type : "")
                                        // }
                                    }
                                }

                                GridLayout {
                                    Layout.fillWidth: true
                                    columns: 2
                                    columnSpacing: 12
                                    rowSpacing: 10

                                    NumberField {
                                        id: front_reflectivity
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "Reflectance (rho)"
                                        minValue: 0
                                        maxValue: 1
                                        decimals: 4
                                        enforceMin: true
                                        enforceMax: true
                                        onValueChanged: if (material_editor.frontEditor) material_editor.frontEditor.reflectivity = value
                                    }

                                    NumberField {
                                        id: front_transmitivity
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "Transmittance (tau)"
                                        minValue: 0
                                        maxValue: 1
                                        decimals: 4
                                        enforceMin: true
                                        enforceMax: true
                                        onValueChanged: if (material_editor.frontEditor) material_editor.frontEditor.transmitivity = value
                                    }

                                    NumberField {
                                        id: front_refraction_front
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "n Front"
                                        minValue: 1
                                        maxValue: 5
                                        decimals: 4
                                        enforceMin: true
                                        onValueChanged: if (material_editor.frontEditor) material_editor.frontEditor.refraction_index_front = value
                                    }

                                    NumberField {
                                        id: front_refraction_back
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "n Back"
                                        minValue: 1
                                        maxValue: 5
                                        decimals: 4
                                        enforceMin: true
                                        onValueChanged: if (material_editor.frontEditor) material_editor.frontEditor.refraction_index_back = value
                                    }

                                    NumberField {
                                        id: front_slope_error
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "Slope Error"
                                        unit: "mrad"
                                        minValue: 0
                                        maxValue: 1000
                                        decimals: 3
                                        enforceMin: true
                                        onValueChanged: if (material_editor.frontEditor) material_editor.frontEditor.slope_error = value
                                    }

                                    NumberField {
                                        id: front_specularity_error
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "Specularity Error"
                                        unit: "mrad"
                                        minValue: 0
                                        maxValue: 1000
                                        decimals: 3
                                        enforceMin: true
                                        onValueChanged: if (material_editor.frontEditor) material_editor.frontEditor.specularity_error = value
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    Button {
                                        Layout.fillWidth: true
                                        text: "Angular Reflectance"
                                        enabled: false
                                    }

                                    Button {
                                        Layout.fillWidth: true
                                        text: "Angular Transmittance"
                                        enabled: false
                                    }
                                }

                                Binding {
                                    target: front_reflectivity
                                    property: "value"
                                    when: !front_reflectivity.textField.activeFocus
                                    value: material_editor.frontEditor ? material_editor.frontEditor.reflectivity : 0
                                }

                                Binding {
                                    target: front_transmitivity
                                    property: "value"
                                    when: !front_transmitivity.textField.activeFocus
                                    value: material_editor.frontEditor ? material_editor.frontEditor.transmitivity : 0
                                }

                                Binding {
                                    target: front_refraction_front
                                    property: "value"
                                    when: !front_refraction_front.textField.activeFocus
                                    value: material_editor.frontEditor ? material_editor.frontEditor.refraction_index_front : 1
                                }

                                Binding {
                                    target: front_refraction_back
                                    property: "value"
                                    when: !front_refraction_back.textField.activeFocus
                                    value: material_editor.frontEditor ? material_editor.frontEditor.refraction_index_back : 1
                                }

                                Binding {
                                    target: front_slope_error
                                    property: "value"
                                    when: !front_slope_error.textField.activeFocus
                                    value: material_editor.frontEditor ? material_editor.frontEditor.slope_error : 0
                                }

                                Binding {
                                    target: front_specularity_error
                                    property: "value"
                                    when: !front_specularity_error.textField.activeFocus
                                    value: material_editor.frontEditor ? material_editor.frontEditor.specularity_error : 0
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            radius: 14
                            color: Qt.rgba(1, 1, 1, 0.03)
                            border.width: 1
                            border.color: Theme.lineColor
                            enabled: material_editor.hasSelection
                            opacity: enabled ? 1.0 : 0.55
                            implicitHeight: back_column.implicitHeight + 28

                            ColumnLayout {
                                id: back_column
                                anchors.fill: parent
                                anchors.margins: 14
                                spacing: 12

                                RowLayout {
                                    Layout.fillWidth: true

                                    Label {
                                        text: "Back Side"
                                        font.pointSize: 15
                                        font.bold: true
                                    }

                                    Item {
                                        Layout.fillWidth: true
                                    }

                                    Button {
                                        text: "Absorber"
                                        enabled: material_editor.hasSelection && material_editor.backEditor
                                        onClicked: material_editor.backEditor.set_ideal_absorption()
                                    }

                                    Button {
                                        text: "Reflector"
                                        enabled: material_editor.hasSelection && material_editor.backEditor
                                        onClicked: material_editor.backEditor.set_ideal_reflection()
                                    }

                                    Button {
                                        text: "Transmitter"
                                        enabled: material_editor.hasSelection && material_editor.backEditor
                                        onClicked: material_editor.backEditor.set_ideal_transmission()
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 6

                                        Body {
                                            text: "Interaction"
                                            font.pointSize: Theme.controlRowLabelSize
                                        }

                                        GlassComboBox {
                                            id: back_interaction
                                            Layout.fillWidth: true
                                            model: material_editor.interactionOptions
                                            borderColor: Theme.lineColor
                                            enabled: material_editor.hasSelection
                                            onActivated: {
                                                if (material_editor.backEditor && currentText.length > 0) {
                                                    material_editor.backEditor.interaction_type = currentText
                                                }
                                            }
                                        }

                                        // Binding {
                                        //     target: back_interaction
                                        //     property: "currentIndex"
                                        //     value: root.comboIndex(material_editor.interactionOptions, material_editor.backEditor ? material_editor.backEditor.interaction_type : "")
                                        // }
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 6

                                        Body {
                                            text: "Error Type"
                                            font.pointSize: Theme.controlRowLabelSize
                                        }

                                        GlassComboBox {
                                            id: back_distribution
                                            Layout.fillWidth: true
                                            model: material_editor.distributionOptions
                                            borderColor: Theme.lineColor
                                            enabled: material_editor.hasSelection && model.rowCount() > 0
                                            onActivated: {
                                                if (material_editor.backEditor && currentText.length > 0) {
                                                    material_editor.backEditor.error_distribution_type = currentText
                                                }
                                            }
                                        }

                                        // Binding {
                                        //     target: back_distribution
                                        //     property: "currentIndex"
                                        //     value: root.comboIndex(material_editor.distributionOptions, material_editor.backEditor ? material_editor.backEditor.error_distribution_type : "")
                                        // }
                                    }
                                }

                                GridLayout {
                                    Layout.fillWidth: true
                                    columns: 2
                                    columnSpacing: 12
                                    rowSpacing: 10

                                    NumberField {
                                        id: back_reflectivity
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "Reflectance (rho)"
                                        minValue: 0
                                        maxValue: 1
                                        decimals: 4
                                        enforceMin: true
                                        enforceMax: true
                                        onValueChanged: if (material_editor.backEditor) material_editor.backEditor.reflectivity = value
                                    }

                                    NumberField {
                                        id: back_transmitivity
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "Transmittance (tau)"
                                        minValue: 0
                                        maxValue: 1
                                        decimals: 4
                                        enforceMin: true
                                        enforceMax: true
                                        onValueChanged: if (material_editor.backEditor) material_editor.backEditor.transmitivity = value
                                    }

                                    NumberField {
                                        id: back_refraction_front
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "n Front"
                                        minValue: 1
                                        maxValue: 5
                                        decimals: 4
                                        enforceMin: true
                                        onValueChanged: if (material_editor.backEditor) material_editor.backEditor.refraction_index_front = value
                                    }

                                    NumberField {
                                        id: back_refraction_back
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "n Back"
                                        minValue: 1
                                        maxValue: 5
                                        decimals: 4
                                        enforceMin: true
                                        onValueChanged: if (material_editor.backEditor) material_editor.backEditor.refraction_index_back = value
                                    }

                                    NumberField {
                                        id: back_slope_error
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "Slope Error"
                                        unit: "mrad"
                                        minValue: 0
                                        maxValue: 1000
                                        decimals: 3
                                        enforceMin: true
                                        onValueChanged: if (material_editor.backEditor) material_editor.backEditor.slope_error = value
                                    }

                                    NumberField {
                                        id: back_specularity_error
                                        Layout.fillWidth: true
                                        fieldWidth: parent.width / 2 - 6
                                        label: "Specularity Error"
                                        unit: "mrad"
                                        minValue: 0
                                        maxValue: 1000
                                        decimals: 3
                                        enforceMin: true
                                        onValueChanged: if (material_editor.backEditor) material_editor.backEditor.specularity_error = value
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    Button {
                                        Layout.fillWidth: true
                                        text: "Angular Reflectance"
                                        enabled: false
                                    }

                                    Button {
                                        Layout.fillWidth: true
                                        text: "Angular Transmittance"
                                        enabled: false
                                    }
                                }

                                Binding {
                                    target: back_reflectivity
                                    property: "value"
                                    when: !back_reflectivity.textField.activeFocus
                                    value: material_editor.backEditor ? material_editor.backEditor.reflectivity : 0
                                }

                                Binding {
                                    target: back_transmitivity
                                    property: "value"
                                    when: !back_transmitivity.textField.activeFocus
                                    value: material_editor.backEditor ? material_editor.backEditor.transmitivity : 0
                                }

                                Binding {
                                    target: back_refraction_front
                                    property: "value"
                                    when: !back_refraction_front.textField.activeFocus
                                    value: material_editor.backEditor ? material_editor.backEditor.refraction_index_front : 1
                                }

                                Binding {
                                    target: back_refraction_back
                                    property: "value"
                                    when: !back_refraction_back.textField.activeFocus
                                    value: material_editor.backEditor ? material_editor.backEditor.refraction_index_back : 1
                                }

                                Binding {
                                    target: back_slope_error
                                    property: "value"
                                    when: !back_slope_error.textField.activeFocus
                                    value: material_editor.backEditor ? material_editor.backEditor.slope_error : 0
                                }

                                Binding {
                                    target: back_specularity_error
                                    property: "value"
                                    when: !back_specularity_error.textField.activeFocus
                                    value: material_editor.backEditor ? material_editor.backEditor.specularity_error : 0
                                }
                            }
                        }
                    }
                }
            }

            Item {
                id: right_side
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.horizontalStretchFactor: 1

                onHeightChanged: {
                    console.log("LH CHANGED", height)
                }

                Q3D.View3D {
                    id: preview_3d
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: width

                    environment: Q3DH.ExtendedSceneEnvironment {
                        antialiasingMode: Q3D.SceneEnvironment.MSAA
                        antialiasingQuality: Q3D.SceneEnvironment.VeryHigh

                        probeExposure: 1.2

                        tonemapMode: Q3D.SceneEnvironment.TonemapModeAces

                        depthOfFieldEnabled: true
                        depthOfFieldBlurAmount: 4
                        depthOfFieldFocusDistance: camera.position.length()
                        //depthOfFieldFocusRange: Math.max(2, preview_3d.geom_bb_max_span * 10)

                        depthOfFieldFocusRange: 100

                        onDepthOfFieldFocusRangeChanged: {
                            console.log("FOCUS", depthOfFieldFocusRange)
                        }

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

                    property real geom_distance : Math.max(geom_bb_max_span/2.0, 2)

                    Q3D.Node {
                        id: camera_host

                        Q3D.PerspectiveCamera {
                            id: camera
                            position: Qt.vector3d(preview_3d.geom_distance, preview_3d.geom_distance, 0.0)
                            //eulerRotation: Qt.vector3d(10, 45, 0)

                            Component.onCompleted: {
                                lookAt(Qt.vector3d(0,0,0))
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
                            duration: 24000
                            loops: Animation.Infinite
                            running: true
                        }
                    }

                    Q3D.DirectionalLight {
                        eulerRotation.x: -45
                        eulerRotation.y: 45
                    }

                    Q3D.Node {
                        rotation: Q3D.Quaternion.fromEulerAngles(-90,0,0)
                        Q3D.Model {
                            id: surface_geometry
                            geometry: App.materials.group_edit.surface_geometry

                            materials : [
                                Q3D.PrincipledMaterial {
                                    metalness: 1
                                    roughness: 0
                                    baseColor: "white"
                                    cullMode: Q3D.Material.NoCulling
                                }
                            ]


                        }
                    }
                }
            }

        }
    }


    //OpticalMaterialsTable {}

    //EndSpacer {}
}


