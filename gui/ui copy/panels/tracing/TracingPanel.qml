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
        header.text: Documentation.tracing.headers.tracing
        body.text: Documentation.tracing.tracing
    }

    GridLayout {
        width: parent.width
        rows: 14
        columns: 2
        columnSpacing: 20
        rowSpacing: 10

        // TRACER SECTION
        DocSection {
            Layout.row: 0
            Layout.column: 0
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.tracer
            body.text: Documentation.tracing.tracer
        }

        CheckBoxField {
            Layout.row: 1
            Layout.column: 0
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            label: "Original Tracer"
            checked: Session.core.tracing.tracerIndex == 0
            onCheckBoxClicked: {
                Session.core.tracing.tracerIndex = 0
            }
        }

        DocSection {
            Layout.row: 1
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.originalTracer
            body.text: Documentation.tracing.originalTracer
        }

        CheckBoxField {
            Layout.row: 2
            Layout.column: 0
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            label: "Refactored Tracer"
            checked: Session.core.tracing.tracerIndex == 1
            onCheckBoxClicked: {
                Session.core.tracing.tracerIndex = 1
            }
        }

        DocSection {
            Layout.row: 2
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.refactorTracer
            body.text: Documentation.tracing.refactorTracer
        }

        CheckBoxField {
            Layout.row: 3
            Layout.column: 0
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            label: "GPU Tracer"
            checked: Session.core.tracing.tracerIndex == 2
            onCheckBoxClicked: {
                Session.core.tracing.tracerIndex = 2
            }
        }

        DocSection {
            Layout.row: 3
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.gpuTracer
            body.text: Documentation.tracing.gpuTracer
        }

        // ENGINE SECTION
        DocSection {
            Layout.row: 4
            Layout.column: 0
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.engine
            body.text: Documentation.tracing.engine
        }

        ScientificNotationField {
            Layout.row: 5
            Layout.column: 0
            Layout.preferredWidth: 200
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            label: "Intersections"
            value: Session.core.tracing.maxIntersections
            mantissaDecimals: 2
            minValue: 1e3
            maxValue: 9.99e10

            onValueChanged: function(newValue) {
                if (newValue) Session.core.tracing.maxIntersections = newValue
            }
        }

        DocSection {
            Layout.row: 5
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.maxIntersections
            body.text: Documentation.tracing.maxIntersections
        }

        ScientificNotationField {
            Layout.row: 6
            Layout.column: 0
            Layout.preferredWidth: 200
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            label: "Max Rays"
            value: Session.core.tracing.maxRays
            mantissaDecimals: 2
            minValue: 1e3
            maxValue: 9.99e10

            onValueChanged: function(newValue) {
                if (newValue) Session.core.tracing.maxRays = newValue
            }
        }

        DocSection {
            Layout.row: 6
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.maxRays
            body.text: Documentation.tracing.maxRays
        }

        CheckBoxField {
            Layout.row: 7
            Layout.column: 0
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            label: "Sun Shape"
            checked: Session.core.tracing.sunShape
            onCheckBoxClicked: {
                Session.core.tracing.sunShape = !Session.core.tracing.sunShape
            }
        }

        DocSection {
            Layout.row: 7
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.sunShape
            body.text: Documentation.tracing.sunShape
        }

        // EXECUTION SECTION
        DocSection {
            Layout.row: 8
            Layout.column: 0
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.execution
            body.text: Documentation.tracing.execution
        }

        NumberField {
            Layout.row: 9
            Layout.column: 0
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            label: "CPU cores"
            unit: "cores"
            fieldWidth: 150
            value: Session.core.tracing.cpuCores

            onValueChanged: {
                Session.core.tracing.cpuCores = value
            }
        }

        DocSection {
            Layout.row: 9
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.cpuCores
            body.text: Documentation.tracing.cpuCores
        }

        NumberField {
            Layout.row: 10
            Layout.column: 0
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            label: "Seed Value"
            fieldWidth: 150
            value: Session.core.tracing.seedValue

            onValueChanged: {
                Session.core.tracing.seedValue = value
            }
        }

        DocSection {
            Layout.row: 10
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.seed
            body.text: Documentation.tracing.seed
        }

        // OPTIMIZATION SECTION
        DocSection {
            Layout.row: 11
            Layout.column: 0
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.optimization
            body.text: Documentation.tracing.optimization
        }

        CheckBoxField {
            Layout.row: 12
            Layout.column: 0
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            label: "Optical Errors"
            checked: Session.core.tracing.opticalErrors
            onCheckBoxClicked: {
                Session.core.tracing.opticalErrors = !Session.core.tracing.opticalErrors
            }
        }

        DocSection {
            Layout.row: 12
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.opticalErrors
            body.text: Documentation.tracing.opticalErrors
        }

        CheckBoxField {
            Layout.row: 13
            Layout.column: 0
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            label: "Point Focus System"
            checked: Session.core.tracing.pointFocusSystem
            onCheckBoxClicked: {
                Session.core.tracing.pointFocusSystem = !Session.core.tracing.pointFocusSystem
            }
        }

        DocSection {
            Layout.row: 13
            Layout.column: 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            header.text: Documentation.tracing.headers.pointFocusSystem
            body.text: Documentation.tracing.pointFocusSystem
        }

        EndSpacer {}
    }
}
