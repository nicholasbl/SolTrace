import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto
Column {
    id: root
    anchors.fill: parent
    anchors.margins: 20
    anchors.rightMargin: 50
    spacing: 10
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
    GlassTabBar {
        model: ["Table View", "List View"]
    }
    SectionCallout {
        title: "Invalid Materials"
        OpticalMaterialsTable {}
    }
    Repeater {
        model: Backend.render_groups_model
        SectionCallout {
            required property string name
            title: name
            OpticalMaterialsTable {}
        }
    }
    SectionCallout {
        title: "All Materials"
        OpticalMaterialsTable {}
    }
    EndSpacer {}
}



