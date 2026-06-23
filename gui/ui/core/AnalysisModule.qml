import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

ColumnLayout {
    id: root

    STPipelineBar {
        id: bar

        currentIndex: Math.min(App.view.analyze_section, 3)
        onCurrentIndexChanged: App.view.analyze_section = currentIndex

        Layout.fillWidth: true

        collapseLabels: App.view.left_panel.size === SplitPanelData.Small

        prefixModel: ["4a", "4b", "4c", "4d"]
        iconModel: ["\uf03a", "\ue4bc", "\uf06d", "\uf019"]
        model: ["Results", "Intersections", "Flux", "Export"]

    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Material.dividerColor
    }

    StackLayout {
        currentIndex: bar.currentIndex
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: 8

        ResultListPane {}

        AnalyzeIntersections {

        }

        AnalyzeFlux {

        }

        AnalyzeExport {

        }
    }

    WorkflowStepper {
        previous: "Run Tracer"
        currentIndex: ViewModule.Analyze
    }
}
