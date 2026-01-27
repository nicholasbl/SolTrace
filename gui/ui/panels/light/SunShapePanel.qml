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
    property string chartTitle: "Sun Shape Profile"

    // Chart properties
    property int chartWidth: 600
    property int chartHeight: 400
    property string xAxisTitle: "Angle from Center (mrad)"
    property real xAxisMin: profiles[currentIndex].xAxisMin
    property real xAxisMax: profiles[currentIndex].xAxisMax
    property string yAxisTitle: "Intensity"
    property real yAxisMin: 0
    property real yAxisMax: 1

    property alias chartView: chartView
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

                    onNumberOfPointsChanged: {
                        if (root.currentIndex === 3) {
                            customSpinBox.value = numberOfPoints
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: chartWidth
            Layout.preferredHeight: chartHeight
            color: "transparent"
            border.width: 1
            border.color: Theme.lineColor
            radius: 8
            Layout.alignment: Qt.AlignTop

            GraphsView {
                id: chartView
                anchors.fill: parent
                anchors.margins: 10
                //title: chartTitle
                //legend.visible: false
                //titleColor: Theme.textColor
                //backgroundColor: "transparent"

                theme: GraphsTheme {
                    colorScheme: GraphsTheme.ColorScheme.Light
                    backgroundVisible: false
                    seriesColors: ["#E0D080", "#B0A060"]
                    borderColors: [Theme.lineColor]
                    plotAreaBackgroundVisible: false
                    grid.mainColor: Theme.lineColor
                    grid.subColor: Theme.lineColor
                    axisY.mainColor: Theme.lineColor
                    axisY.subColor: Theme.lineColor
                    axisX.mainColor: Theme.lineColor
                    axisX.subColor: Theme.lineColor
                    labelTextColor: Theme.textColor
                }


                axisX: ValueAxis {
                    id: xAxis
                    titleText: xAxisTitle
                    min: xAxisMin
                    max: xAxisMax
                    //color: Theme.textColor
                    //labelsColor: Theme.textColor
                    //shadesColor: Theme.textColor
                    //gridLineColor: Theme.lineColor
                    //gridVisible: false
                    //minorGridVisible: false
                }

                axisY: ValueAxis {
                    id: yAxis
                    titleText: yAxisTitle
                    min: yAxisMin
                    max: yAxisMax
                    //color: Theme.textColor
                    //labelsColor: Theme.textColor
                    //shadesColor: Theme.textColor
                    //gridLineColor: Theme.lineColor
                }

                LineSeries {
                    id: lineSeries
                    name: ""
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }
    }
}
