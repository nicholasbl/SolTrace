import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

import QtQuick3D as Q3D
import QtQuick3D.Helpers as Q3DH

ColumnLayout {
    id: root

    spacing: 10

    property string current_material_name : App.materials.current_material_name

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

    STLabel {
        visible: root.current_material_name.length === 0
        text: "No Material Selected"
        font.pointSize: 17

        Layout.fillWidth: true
        Layout.margins: 10
    }

    GlassTextField {
        visible: root.current_material_name.length !== 0
        text: root.current_material_name

        onAccepted: function(text) {
            root.current_material_name = text
        }

    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        //Layout.minimumHeight: width

        //Component.onCompleted: console.log("RH CHANGED", height, width)

        // Track weirdness
        onHeightChanged: {
            console.log("RH CHANGED", height, width)
        }
        onWidthChanged: {
            console.log("RH CHANGED", height, width)
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10

            MaterialLeftSide {
                id: left_side
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
            }

            MaterialRightSide {
                id: right_side
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
            }

        }
    }


    //OpticalMaterialsTable {}

    //EndSpacer {}
}


