import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import SolTraceProto

Item {
    id: root
    property var source: null
    property int iconSize: 22
    property real horizontalMargin: 25
    property real verticalMargin: 0
    property real iconSpacing: 5

    width: parent.width
    height: scrollView.height

    property QtObject icon: QtObject{
        property int size: 22
        property int activeSize: 50
    }

    property list<QtObject> model: [
        QtObject {
            property string name: "Configuration"
            property list<QtObject> panels: [
                QtObject {
                    property string name: "Sun"
                    property string icon: "qrc:/icons/assets/icons/sun.svg"
                },
                QtObject {
                    property string name: "Materials"
                    property string icon: "qrc:/icons/assets/icons/blocks.svg"
                },
                // QtObject {
                //     property string name: "Tracing"
                //     property string icon: "qrc:/icons/assets/icons/placeholder.svg"
                // },
                QtObject {
                    property string name: "Layout"
                    property string icon: "qrc:/icons/assets/icons/placeholder.svg"
                }
            ]
        },
        QtObject {
            property string name: "Simulation"
            property list<QtObject> panels: [
                QtObject {
                    property string name: "Execution"
                    property string icon: "qrc:/icons/assets/icons/placeholder.svg"
                },
                QtObject {
                    property string name: "Navigation"
                    property string icon: "qrc:/icons/assets/icons/play.svg"
                },
                /* QtObject {
                    property string name: "Monitor"
                    property string icon: "qrc:/icons/assets/icons/placeholder.svg"
                }, */
                QtObject {
                    property string name: "Diagnostics"
                    property string icon: "qrc:/icons/assets/icons/placeholder.svg"
                }
            ]
        },
        QtObject {
            property string name: "Analysis"
            property list<QtObject> panels: [
                QtObject {
                    property string name: "Intersections"
                    property string icon: "qrc:/icons/assets/icons/placeholder.svg"
                },
                QtObject {
                    property string name: "Flux"
                    property string icon: "qrc:/icons/assets/icons/placeholder.svg"
                }
            ]
        }
    ]

    // Phase title component
    component PhaseTitle: Label {
        font.family: "Roboto"
        font.capitalization: Font.AllUppercase
        anchors.verticalCenter: parent.verticalCenter
    }

    // Vertical divider component
    component VerticalDivider: Rectangle {
        width: 1
        height: parent.height - 20
        color: "white"
        opacity: 0.5
        anchors.verticalCenter: parent.verticalCenter
    }

    // Arrow separator component
    component ArrowSeparator: GlassRectangle {
        width: pipeline.height
        height: width
        radius: 20
        anchors.verticalCenter: parent.verticalCenter

        Image {
            id: icon
            source: "qrc:/icons/assets/icons/double-chevron-right.svg"
            width: 16
            height: 16
            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectFit
            smooth: true
            cache: true
            anchors.centerIn: parent
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent

        ScrollBar.horizontal.policy: ScrollBar.AsNeeded
        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        clip: true

        Row {
            id: pipeline
            spacing: 15
            x: Math.max(0, (scrollView.width - width) / 2)


            // Configure Phase
            GlassRectangle {
                width: configureRow.width + 2 * root.horizontalMargin
                height: configureRow.height + 2 * root.verticalMargin
                radius: 25

                Row {
                    id: configureRow
                    spacing: 10
                    anchors.centerIn: parent

                    PhaseTitle {
                        text: root.model[0].name
                    }

                    VerticalDivider {}

                    Row {
                        spacing: root.iconSpacing
                        Repeater {
                            model: root.model[0].panels
                            IconButton {
                                required property int index
                                required property QtObject modelData

                                source: modelData.icon
                                label: modelData.name

                                onClicked: {
                                    Session.core.workflowIndex = 0
                                    Session.core.sectionIndex = index
                                }
                            }
                        }
                    }
                }
            }

            ArrowSeparator {}

            // Simulate Phase
            GlassRectangle {
                width: simulateRow.width + 2 * root.horizontalMargin
                height: simulateRow.height + 2 * root.verticalMargin
                radius: 25

                Row {
                    id: simulateRow
                    spacing: 10
                    anchors.centerIn: parent

                    PhaseTitle {
                        text: root.model[1].name
                    }

                    VerticalDivider {}

                    Row {
                        spacing: 0
                        Repeater {
                            model: root.model[1].panels
                            IconButton {
                                required property int index
                                required property QtObject modelData

                                source: modelData.icon
                                label: modelData.name

                                onClicked: {
                                    Session.core.workflowIndex = 1
                                    Session.core.sectionIndex = index
                                }
                            }
                        }
                    }
                }
            }

            ArrowSeparator {}

            // Analyze Phase
            GlassRectangle {
                width: analyzeRow.width + 2 * root.horizontalMargin
                height: analyzeRow.height + 2 * root.verticalMargin
                radius: 25

                Row {
                    id: analyzeRow
                    spacing: 10
                    anchors.centerIn: parent

                    PhaseTitle {
                        text: root.model[2].name
                    }

                    VerticalDivider {}

                    Row {
                        spacing: root.iconSpacing

                        Repeater {
                            model: root.model[2].panels
                            IconButton {
                                required property int index
                                required property QtObject modelData

                                source: modelData.icon
                                label: modelData.name

                                onClicked: {
                                    Session.core.workflowIndex = 2
                                    Session.core.sectionIndex = index
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
