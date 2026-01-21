import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto
Column {
    id: root
    width: parent.width
    spacing: 10
    RowLayout {
        width: parent.width
        spacing: 40
        Column {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            spacing: 10
            visible: Settings.showDocumentation
            DocSection {
                width: parent.width
                header.text: Documentation.sun.headers.directionalSun
                body.text: Documentation.sun.directionalSun
            }
            DocSection {
                width: parent.width
                header.text: Documentation.sun.headers.calculators
                body.text: Documentation.sun.calculators
            }
        }
        Column {
            spacing: 20
            Layout.alignment: Qt.AlignTop
            visible: Settings.showDocumentation
            Column {
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter
                Image {
                    id: diagram
                    source: "qrc:/images/assets/images/directional_sun.svg"
                    sourceSize.width: 90
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: !Settings.showDocumentation ? 0 : implicitHeight
                }
                Body {
                    width: 280
                    text: Documentation.sun.directionalSunDiagramCaption
                    font.pointSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            Image {
                id: equations
                source: "qrc:/images/assets/images/directional_sun_eq.svg"
                sourceSize.width: 240
                fillMode: Image.PreserveAspectFit
                smooth: true
                anchors.horizontalCenter: parent.horizontalCenter
                height: !Settings.showDocumentation ? 0 : implicitHeight
            }
        }
    }
    GlassTabBar {
        model: ["Legacy", "Duffie", "SOLPOS", "SPA"]
        index: Session.core.sun.calculatorIndex
        onIndexRequested: (newIndex) => Session.core.sun.calculatorIndex = newIndex
    }
    Spacer {
        visible: Settings.showDocumentation
    }
    Item {
        width: parent.width
        height: childrenRect.height
        LegacyCalculatorPanel {
            width: parent.width
            visible: Session.core.sun.calculatorIndex === 0
            height: visible ? implicitHeight : 0
        }
        DuffieCalculatorPanel {
            width: parent.width
            visible: Session.core.sun.calculatorIndex === 1
            height: visible ? implicitHeight : 0
        }
        SOLPOSCalculatorPanel {
            width: parent.width
            visible: Session.core.sun.calculatorIndex === 2
            height: visible ? implicitHeight : 0
        }
        SPACalculatorPanel {
            width: parent.width
            visible: Session.core.sun.calculatorIndex === 3
            height: visible ? implicitHeight : 0
        }
    }
    Spacer {}
}
