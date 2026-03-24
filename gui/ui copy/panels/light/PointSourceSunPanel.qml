import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto
Column {
    width: parent.width
    spacing: 10
    RowLayout {
        width: parent.width
        spacing: 40
        Column {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            spacing: 10
            DocSection {
                width: parent.width
                header.text: Documentation.sun.headers.pointSourceSun
                body.text: Documentation.sun.pointSourceSun
            }
            Row {
                spacing: 10
                x: !UserSettings.showDocumentation ? 0 : (parent.width - width) / 2
                Column {
                    spacing: 5
                    NumberField {
                        unitLabel: "X"
                        fieldWidth: 150
                        value: Session.core.sun.pointSourceSun.x
                        decimals: 2
                        onValueChanged: function(newValue) {
                            if (newValue) Session.core.sun.pointSourceSun.x = newValue
                        }
                    }
                }
                Column {
                    spacing: 5
                    NumberField {
                        unitLabel: "Y"
                        fieldWidth: 150
                        value: Session.core.sun.pointSourceSun.y
                        decimals: 2
                        onValueChanged: function(newValue) {
                            if (newValue) Session.core.sun.pointSourceSun.y = newValue
                        }
                    }
                }
                Column {
                    spacing: 5
                    NumberField {
                        unitLabel: "Z"
                        fieldWidth: 150
                        value: Session.core.sun.pointSourceSun.z
                        decimals: 2
                        onValueChanged: function(newValue) {
                            if (newValue) Session.core.sun.pointSourceSun.z = newValue
                        }
                    }
                }
            }
        }
        Row {
            spacing: 20
            Layout.alignment: Qt.AlignTop
            visible: UserSettings.showDocumentation
            Image {
                id: equations
                source: "qrc:/images/assets/images/point_source_sun_eq.svg"
                sourceSize.width: 240
                fillMode: Image.PreserveAspectFit
                smooth: true
                anchors.verticalCenter: parent.verticalCenter
                height: !UserSettings.showDocumentation ? 0 : implicitHeight
            }
            Column {
                spacing: 10
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    id: diagram
                    source: "qrc:/images/assets/images/point_source.svg"
                    sourceSize.width: 170
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: !UserSettings.showDocumentation ? 0 : implicitHeight
                }
                Body {
                    width: 280
                    text: Documentation.sun.pointSourceSunDiagramCaption
                    font.pointSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }
}
