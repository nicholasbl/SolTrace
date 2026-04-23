import QtQuick
import QtQuick
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts
import QtGraphs

import SolTrace

Flickable {
    id: root
    property var left_panel_size: App.view.left_panel.size
    property var sun_module : App.sun

    contentWidth: width
    contentHeight: content_column.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    ColumnLayout {
        id: content_column
        width: root.width

        InlineDocumentation {
            key: "configure.sun"
            target: App.view.left_panel

            Layout.margins: 8
        }


        STComboBar {
            id: bar
            currentIndex: App.view.sun_section
            onCurrentIndexChanged: App.view.sun_section = currentIndex

            Layout.fillWidth: true

            fontFamily: App.view.left_panel.size < 1 ? "Font Awesome 7 Free" : ""

            model: App.view.left_panel.size < 1 ?
                        ["\uf0eb", "\uf06a"]
                      :
                        ["Type & Position", "Shape"]
        }

        StackLayout {
            currentIndex: App.view.sun_section

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                STComboBar {
                    Layout.fillWidth: true

                    currentIndex: App.sun.type
                    onCurrentIndexChanged: App.sun.type = currentIndex

                    fontFamily: App.view.left_panel.size < 1 ? "Font Awesome 7 Free" : ""

                    model: App.view.left_panel.size < 1 ?
                                ["\uf06a", "\uf06a"]
                              :
                                ["Directional Sun", "Point Source"]
                }

                InlineDocumentation {
                    Layout.fillWidth: true
                    Layout.margins: 8

                    key: "configure.sun.type." + ["directional", "point_source"][App.sun.type]
                    target: App.view.left_panel
                }


                ColumnLayout {
                    Layout.fillWidth: true
                    visible: App.sun.type == SunModule.Directional

                    InlineDocumentation {
                        Layout.fillWidth: true
                        Layout.margins: 8
                        key: "configure.sun.calculators.legacy"
                        target: App.view.left_panel
                    }

                    LocationField {
                        id: location
                        Layout.fillWidth: true
                        onModified: {
                            App.sun.calc_data.latitude = latitude
                            App.sun.calc_data.longitude = longitude
                        }
                    }

                    DateTimeField {
                        id: date
                        Layout.fillWidth: true
                        onModified: {
                            App.sun.calc_data.month = month
                            App.sun.calc_data.day = day
                            App.sun.calc_data.hour = hour
                            App.sun.calc_data.minute = minute
                        }
                    }
                }

                STPropertyPanel {
                    Layout.fillWidth: true

                    title: "Manual Position"
                    collapsible: false
                    visible: App.sun.type == SunModule.PointSource

                    GridLayout {
                        width: parent.width

                        STSpinBoxField {
                            Layout.row: 0
                            Layout.column: 0

                            Layout.fillWidth: true
                            Layout.preferredWidth: 100
                            value: App.sun.position.x
                            onValueModified: {
                                App.sun.position.x = value
                                App.sun.position.from_calculator = false
                            }
                            label: "X"
                            from: -1000000
                            to: 1000000
                        }

                        STSpinBoxField {
                            Layout.row: App.view.left_panel.size < 1 ? 1 : 0
                            Layout.column: App.view.left_panel.size < 1 ? 0 : 1

                            Layout.fillWidth: true
                            Layout.preferredWidth: 100
                            label: "Y"
                            value: App.sun.position.y
                            onValueModified: {
                                App.sun.position.y = value
                                App.sun.position.from_calculator = false
                            }
                            from: -1000000
                            to: 1000000
                        }

                        STSpinBoxField {
                            Layout.row: App.view.left_panel.size < 1 ? 2 : 0
                            Layout.column: App.view.left_panel.size < 1 ? 0 : 2

                            Layout.fillWidth: true
                            Layout.preferredWidth: 100
                            label: "Z"
                            value: App.sun.position.z
                            onValueModified: {
                                App.sun.position.z = value
                                App.sun.position.from_calculator = false
                            }
                            from: -1000000
                            to: 1000000
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                STComboBar {
                    Layout.fillWidth: true
                    currentIndex: App.sun.shape.shape
                    onCurrentIndexChanged: App.sun.shape.shape = currentIndex
                    fontFamily: App.view.left_panel.size < 1 ? "Font Awesome 7 Free" : ""
                    model: App.view.left_panel.size < 1 ?
                                ["\uf06a", "\uf06a", "\uf06a", "\uf06a"]
                              :
                                ["Gaussian", "Pillbox", "CSR", "Custom"]
                }

                InlineDocumentation {
                    Layout.fillWidth: true
                    Layout.margins: 8
                    key: "configure.sun.shape." + ["gaussian", "pillbox", "csr", "custom"][App.sun.shape.shape]
                    target: App.view.left_panel
                }

                GridLayout {
                    rows: 2
                    columns: 2
                    Layout.margins: 12

                    StackLayout {
                        currentIndex: App.sun.shape.shape
                        Layout.row: App.view.left_panel.size < 2 ? 1 : 0
                        Layout.column: App.view.left_panel.size < 2 ? 0 : 1
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 200

                        STSpinBoxField {
                            Layout.fillWidth: true
                            label: "Standard Deviation"
                            value: App.sun.shape.sigma
                            decimals: 3
                            onValueChanged: { App.sun.shape.sigma = value }
                        }

                        STSpinBoxField {
                            Layout.fillWidth: true
                            label: "Half-width"
                            value: App.sun.shape.half_width
                            decimals: 3
                            onValueChanged: { App.sun.shape.half_width = value }
                        }

                        STSpinBoxField {
                            Layout.fillWidth: true
                            label: "Circumsolar Ratio"
                            value: App.sun.shape.csr
                            decimals: 3
                            onValueChanged: { App.sun.shape.csr = value }
                        }

                        CustomSunShapeTable { }
                    }

                    SunShapeGraph {
                        Layout.row: 0
                        Layout.column: 0
                        Layout.preferredWidth: 300
                        Layout.preferredHeight: 400
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        title: "Emission Profile"
                        xAxisTitle: "Angle (mrad)"
                        yAxisTitle: "Intensity"
                    }
                }
            }
        }
    }

    ScrollBar.vertical: ScrollBar {
        id: vbar
    }
}
