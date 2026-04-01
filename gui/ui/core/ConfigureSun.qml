import QtQuick
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts
import QtGraphs

import SolTrace

ColumnLayout {
    property var left_panel_size: App.view.left_panel_size
    property var sun_module : App.sun

    STComboBar {
        id: bar
        currentIndex: App.view.sun_section
        onCurrentIndexChanged: App.view.sun_section = currentIndex

        Layout.fillWidth: true

        fontFamily: App.view.left_panel_size < 1 ? "Font Awesome 7 Free" : ""

        model: App.view.left_panel_size < 1 ?
                    ["\uf0eb", "\uf53f"]
                  :
                    ["Shape", "Position"]

    }

    StackLayout {
        currentIndex: App.view.sun_section

        Layout.fillWidth: true
        Layout.fillHeight: true

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            STComboBar {
                Layout.fillWidth: true

                currentIndex: App.sun.definition.type
                onCurrentIndexChanged: App.sun.definition.type = currentIndex

                fontFamily: App.view.left_panel_size < 1 ? "Font Awesome 7 Free" : ""

                model: App.view.left_panel_size < 1 ?
                            ["\uf06a", "\uf06a"]
                          :
                            ["Directional Sun", "Point Source"]

            }

            STComboBar {
                Layout.fillWidth: true

                currentIndex: App.sun.definition.shape
                onCurrentIndexChanged: App.sun.definition.shape = currentIndex

                fontFamily: App.view.left_panel_size < 1 ? "Font Awesome 7 Free" : ""

                model: App.view.left_panel_size < 1 ?
                            ["\uf06a", "\uf06a", "\uf06a", "\uf06a"]
                          :
                            ["Gaussian", "Pillbox", "CSR", "Custom"]

            }

            StackLayout {
                currentIndex: App.sun.definition.shape

                Layout.fillWidth: true
                Layout.fillHeight: true

                STSpinBoxField {
                    Layout.fillWidth: App.view.left_panel_size < 1
                    Layout.preferredWidth: App.view.left_panel_size < 1 ? null : 200

                    label: "Standard Deviation"
                    decimals: 3
                }

                STSpinBoxField {
                    Layout.fillWidth: App.view.left_panel_size < 1
                    Layout.preferredWidth: App.view.left_panel_size < 1 ? null : 200

                    label: "Half-width"
                    decimals: 3
                }

                STSpinBoxField {
                    Layout.fillWidth: App.view.left_panel_size < 1
                    Layout.preferredWidth: App.view.left_panel_size < 1 ? null : 200

                    label: "Circumsolar Ratio"
                    decimals: 3
                }

                STSpinBoxField {
                    Layout.fillWidth: App.view.left_panel_size < 1
                    Layout.preferredWidth: App.view.left_panel_size < 1 ? null : 200

                    label: "Number of Points"
                    from: 0
                }

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
