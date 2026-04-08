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
                        ["\uf0eb", "\uf53f"]
                      :
                        ["Shape", "Type", " Position"]

        }

        StackLayout {
            currentIndex: App.view.sun_section

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true


                STComboBar {
                    Layout.fillWidth: true

                    currentIndex: App.sun.definition.shape
                    onCurrentIndexChanged: App.sun.definition.shape = currentIndex

                    fontFamily: App.view.left_panel.size < 1 ? "Font Awesome 7 Free" : ""

                    model: App.view.left_panel.size < 1 ?
                                ["\uf06a", "\uf06a", "\uf06a", "\uf06a"]
                              :
                                ["Gaussian", "Pillbox", "CSR", "Custom"]

                }

                InlineDocumentation {
                    Layout.fillWidth: true
                    Layout.margins: 8
                    key: "configure.sun.shape." + ["gaussian", "pillbox", "csr", "custom"][App.sun.definition.shape]
                    target: App.view.left_panel
                }

                StackLayout {
                    currentIndex: App.sun.definition.shape

                    Layout.fillWidth: true

                    STSpinBoxField {
                        Layout.fillWidth: App.view.left_panel.size < 1
                        Layout.preferredWidth: App.view.left_panel.size < 1 ? null : 200

                        label: "Standard Deviation"
                        decimals: 3
                    }

                    STSpinBoxField {
                        Layout.fillWidth: App.view.left_panel.size < 1
                        Layout.preferredWidth: App.view.left_panel.size < 1 ? null : 200

                        label: "Half-width"
                        decimals: 3
                    }

                    STSpinBoxField {
                        Layout.fillWidth: App.view.left_panel.size < 1
                        Layout.preferredWidth: App.view.left_panel.size < 1 ? null : 200

                        label: "Circumsolar Ratio"
                        decimals: 3
                    }

                    STSpinBoxField {
                        Layout.fillWidth: App.view.left_panel.size < 1
                        Layout.preferredWidth: App.view.left_panel.size < 1 ? null : 200

                        label: "Number of Points"
                        from: 0
                    }

                }

                SunShapeGraph {
                    Layout.preferredWidth: 300
                    Layout.preferredHeight: 400
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    title: "Emission Profile"
                    xAxisTitle: "Angle (mrad)"
                    yAxisTitle: "Intensity"
                    xMin: 0; xMax: 10
                    yMin: 0; yMax: 1
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                InlineDocumentation {
                    Layout.fillWidth: true
                    Layout.margins: 8

                    key: "configure.sun.type." + ["directional", "point_source"][App.sun.definition.type]
                    target: App.view.left_panel
                }

                STComboBar {
                    Layout.fillWidth: true

                    currentIndex: App.sun.definition.type
                    onCurrentIndexChanged: App.sun.definition.type = currentIndex

                    fontFamily: App.view.left_panel.size < 1 ? "Font Awesome 7 Free" : ""

                    model: App.view.left_panel.size < 1 ?
                                ["\uf06a", "\uf06a"]
                              :
                                ["Directional Sun", "Point Source"]

                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                LocationField {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                DateTimeField {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }

    ScrollBar.vertical: ScrollBar {
        id: vbar
    }
}
