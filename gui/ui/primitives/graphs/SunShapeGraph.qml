import QtQuick 2.15
import QtQuick.Layouts
import QtGraphs

Rectangle {
    id: root

    // Graph content
    property string title: ""
    property string xAxisTitle: ""
    property string yAxisTitle: ""

    // Axis ranges
    property real xMin: 0
    property real xMax: 1
    property real yMin: 0
    property real yMax: 1

    // Series access
    property alias series: lineSeries

    color: "transparent"
    radius: 8

    Column {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 0

        Text {
            id: graphTitle
            text: root.title
            color: "white"
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: yAxisLabel.height + yAxisLabel.anchors.leftMargin
            height: implicitHeight
        }

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
                    plotAreaBackgroundVisible: false
                    seriesColors: ["#E0D080", "#B0A060"]
                    borderColors: ["#e3e3e3", "#e3e3e3"]
                    grid.mainColor: "#e3e3e3"
                    grid.subColor: "#e3e3e3"
                    grid.mainWidth: 1
                    grid.subWidth: 1
                    axisX.mainColor: "#e3e3e3"
                    axisX.subColor: "#e3e3e3"
                    axisX.labelTextColor: "#e3e3e3"
                    axisX.mainWidth: 1
                    axisX.subWidth: 1
                    axisY.mainColor: "#e3e3e3"
                    axisY.subColor: "#e3e3e3"
                    axisY.labelTextColor: "#e3e3e3"
                    axisY.mainWidth: 1
                    axisY.subWidth: 1
                    labelTextColor: "#e3e3e3"
                }

                axisX: ValueAxis {
                    id: xAxis
                    titleVisible: false
                    min: root.xMin
                    max: root.xMax
                }

                axisY: ValueAxis {
                    id: yAxis
                    titleVisible: false
                    min: root.yMin
                    max: root.yMax
                }

                LineSeries {
                    id: lineSeries
                    name: ""
                }
            }

            Text {
                id: yAxisLabel
                text: root.yAxisTitle
                color: "white"
                rotation: -90
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.verticalCenter: graphView.verticalCenter
                anchors.verticalCenterOffset: -(xAxisLabel.height + xAxisLabel.anchors.bottomMargin)
            }

            Text {
                id: xAxisLabel
                text: root.xAxisTitle
                color: "white"
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.horizontalCenter: graphView.horizontalCenter
                anchors.horizontalCenterOffset: yAxisLabel.height + yAxisLabel.anchors.leftMargin
            }
        }
    }
}
