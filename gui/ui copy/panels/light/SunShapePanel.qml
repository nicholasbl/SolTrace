import QtQuick 2.15
import QtQuick.Layouts
import QtGraphs
import SolTraceProto

Column {
    id: root

    property list<QtObject> profiles: [
        QtObject {
            property string headerText: Documentation.sun.headers.gaussianProfile
            property string explanationText: Documentation.sun.gaussianProfile
            property string controlLabel: "Standard Deviation"
            property real minValue: 0.01
            property real maxValue: 15.0
            property int decimals: 3
            property real value: Session.core.sun.shape.gaussian.stdValue
            property real xAxisMin: -20
            property real xAxisMax: 20
        },
        QtObject {
            property string headerText: Documentation.sun.headers.pillboxProfile
            property string explanationText: Documentation.sun.pillboxProfile
            property string controlLabel: "Half-width"
            property real minValue: 0.01
            property real maxValue: 15.0
            property int decimals: 3
            property real value: Session.core.sun.shape.pillbox.halfWidthValue
            property real xAxisMin: -15
            property real xAxisMax: 15
        },
        QtObject {
            property string headerText: Documentation.sun.headers.buieProfile
            property string explanationText: Documentation.sun.buieProfile
            property string controlLabel: "Circumsolar Ratio (CSR)"
            property real minValue: 0.01
            property real maxValue: 15.0
            property int decimals: 3
            property real value: Session.core.sun.shape.buie.circumsolarRatioValue
            property real xAxisMin: -15
            property real xAxisMax: 15
        },
        QtObject {
            property string headerText: Documentation.sun.headers.customProfile
            property string explanationText: Documentation.sun.customProfile
            property string controlLabel: "# of Points"
            property int minValue: 1
            property int maxValue: 1000
            property int decimals: 0
            property int value: Session.core.sun.shape.custom.numberOfPoints
            property real xAxisMin: -20
            property real xAxisMax: 20
        }
    ]

    // Current profile properties - these update when index changes
    property int currentIndex: Session.core.sun.sunShapeIndex
    property string headerText: profiles[currentIndex].headerText
    property string explanationText: profiles[currentIndex].explanationText
    property string controlLabel: profiles[currentIndex].controlLabel
    property string graphTitle: "Sun Shape Profile"

    // graph properties
    property int graphWidth: 600
    property int graphHeight: 400
    property string xAxisTitle: "Angle from Center (mrad)\n"
    property real xAxisMin: profiles[currentIndex].xAxisMin
    property real xAxisMax: profiles[currentIndex].xAxisMax
    property string yAxisTitle: "<span style='margin-bottom: 100px'>Intensity</span>"
    property real yAxisMin: 0
    property real yAxisMax: 1

    property alias graphView: graphView
    property alias lineSeries: lineSeries

    spacing: !UserSettings.showDocumentation ? 0 : 10

    onCurrentIndexChanged: {
        if (currentIndex !== 3) {
            numberField.value = profiles[currentIndex].value
        }
    }

    DocSection {
        header.text: headerText
        body.text: explanationText
    }

    RowLayout {
        id: controlRow
        width: parent.width
        spacing: !UserSettings.showDocumentation ? 20 : 40

        Item {
            Layout.fillWidth: UserSettings.showDocumentation
            Layout.preferredHeight: UserSettings.showDocumentation ? 0 : 1
            visible: UserSettings.showDocumentation
        }

        Column {
            id: control
            Layout.preferredWidth: root.currentIndex == 3 ? 500 : 200
            Layout.alignment: !UserSettings.showDocumentation || root.currentIndex == 3 ? Qt.AlignTop : Qt.AlignCenter
            spacing: 10

            // NumberField for Gaussian, Pillbox, Buie (indices 0-2)
            NumberField {
                id: numberField
                visible: root.currentIndex !== 3
                label: root.controlLabel
                fieldWidth: 200
                minValue: profiles[root.currentIndex].minValue
                maxValue: profiles[root.currentIndex].maxValue
                decimals: profiles[root.currentIndex].decimals
                value: profiles[root.currentIndex].value

                Component.onCompleted: {
                    value = profiles[root.currentIndex].value
                }

                onValueChanged: {
                    if (root.currentIndex === 0) {
                        Session.core.sun.shape.gaussian.stdValue = value
                    } else if (root.currentIndex === 1) {
                        Session.core.sun.shape.pillbox.halfWidthValue = value
                    } else if (root.currentIndex === 2) {
                        Session.core.sun.shape.buie.circumsolarRatioValue = value
                    }
                }
            }

            // GlassSpinBox for Custom (index 3) - Integer only
            Column {
                visible: root.currentIndex === 3
                spacing: 10

                Body {
                    rawText: root.controlLabel
                    font.pointSize: Theme.controlRowLabelSize
                    width: customSpinBox.width
                }

                GlassSpinBox {
                    id: customSpinBox
                    width: 200
                    stepSize: 1
                    editable: true
                    from: profiles[3].minValue
                    to: profiles[3].maxValue
                    value: profiles[3].value

                    onValueModified: {
                        Session.core.sun.shape.custom.numberOfPoints = value
                        customEmissionProfileTable.numberOfPoints = value
                    }
                }

                CustomEmissionProfileTable {
                    id: customEmissionProfileTable

                    // onNumberOfPointsChanged: {
                    //     if (root.currentIndex === 3) {
                    //         customSpinBox.value = numberOfPoints
                    //     }
                    // }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: graphWidth
            Layout.preferredHeight: graphHeight
            color: "transparent"
            border.width: 1
            border.color: Theme.lineColor
            radius: 8
            Layout.alignment: Qt.AlignTop

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 0

                // Graph Title
                Text {
                    id: graphTitle
                    text: "Emission Profile"
                    color: "white"
                    font.bold: true
                    font.pointSize: Theme.controlRowLabelSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.horizontalCenterOffset: yAxisLabel.height + yAxisLabel.anchors.leftMargin
                    height: implicitHeight
                }

                // Graph and axis labels grouped together
                Item {
                    width: parent.width
                    height: parent.height - graphTitle.height - parent.spacing

                    GraphsView {
                        id: graphView
                        anchors.fill: parent
                        marginTop: 10
                        marginLeft: 25
                        marginBottom: 12

                        theme: GraphsTheme {
                            colorScheme: GraphsTheme.ColorScheme.Dark
                            backgroundVisible: false
                            seriesColors: ["#E0D080", "#B0A060"]
                            borderColors: ["#e3e3e3", "#e3e3e3"]
                            plotAreaBackgroundVisible: false
                            grid.mainColor: "#e3e3e3"
                            grid.subColor: "#e3e3e3"
                            axisY.mainColor: "#e3e3e3"
                            axisY.subColor: "#e3e3e3"
                            axisY.labelTextColor: "#e3e3e3"
                            axisX.mainColor: "#e3e3e3"
                            axisX.subColor: "#e3e3e3"
                            axisX.labelTextColor: "#e3e3e3"
                            labelTextColor: "#e3e3e3"
                            grid.mainWidth: 1
                            grid.subWidth: 1
                            axisX.mainWidth: 1
                            axisX.subWidth: 1
                            axisY.mainWidth: 1
                            axisY.subWidth: 1
                        }

                        axisX: ValueAxis {
                            id: xAxis
                            titleVisible: false
                            min: root.xAxisMin
                            max: root.xAxisMax
                        }

                        axisY: ValueAxis {
                            id: yAxis
                            titleVisible: false
                            min: root.yAxisMin
                            max: root.yAxisMax
                        }

                        LineSeries {
                            id: lineSeries
                            name: ""
                        }
                    }

                    // Custom Y-axis label
                    Text {
                        id: yAxisLabel
                        text: root.yAxisTitle
                        color: "white"
                        font.pointSize: Theme.controlLabelSize
                        rotation: -90
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                        anchors.verticalCenter: graphView.verticalCenter
                        anchors.verticalCenterOffset: -(xAxisLabel.height + xAxisLabel.anchors.bottomMargin)
                    }
                    // Custom X-axis label
                    Text {
                        id: xAxisLabel
                        text: root.xAxisTitle
                        color: "white"
                        font.pointSize: Theme.controlLabelSize
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        anchors.horizontalCenter: graphView.horizontalCenter
                        anchors.horizontalCenterOffset: yAxisLabel.height + yAxisLabel.anchors.leftMargin
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
