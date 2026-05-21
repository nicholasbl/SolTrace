import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

ScrollView {
    id: themeScroll

    Layout.fillWidth: true
    Layout.fillHeight: true
    contentWidth: availableWidth
    clip: true

    ColumnLayout {
        width: themeScroll.availableWidth
        spacing: 12

        Header {
            text: "Theme"
        }

        STPropertyPanel {
            Layout.fillWidth: true

            title: "Glass"
            collapsed: false

            ColumnLayout {
                ColorPickerField {
                    id: glassColorPicker
                    color: App.theme.glassColor
                    label: "Glass Color"
                    onUpdated: {
                        App.theme.glassColor.r = glassColorPicker.color.r
                        App.theme.glassColor.g = glassColorPicker.color.g
                        App.theme.glassColor.b = glassColorPicker.color.b
                    }
                }

                SliderField {
                    Layout.preferredWidth: 250
                    Layout.maximumWidth: 350

                    from: 0
                    to: 100
                    value: App.theme.glassColor.a * 100
                    onModified: {
                        App.theme.glassColor.a = value / 100
                    }

                    text: "Glass Translucency"
                }
            }
        }

        STPropertyPanel {
            Layout.fillWidth: true

            title: "Font"
            collapsed: false

            ColumnLayout {
                ColorPickerField {
                    id: fontColorPicker

                    label: "Font Color"
                    onUpdated: {
                        App.theme.fontColor.r = fontColorPicker.color.r
                        App.theme.fontColor.g = fontColorPicker.color.g
                        App.theme.fontColor.b = fontColorPicker.color.b
                    }
                }

                STSpinBoxField {
                    label: "Header Size"
                    from: 1
                    to: 100
                    Layout.preferredWidth: 200
                    value: App.theme._headerSize
                    onValueModified: {
                        App.theme._headerSize = value
                    }
                }

                STSpinBoxField {
                    label: "Subheader Size"
                    from: 1
                    to: 100
                    Layout.preferredWidth: 200
                    value: App.theme._subHeaderSize
                    onValueModified: {
                        App.theme._subHeaderSize = value
                    }
                }

                STSpinBoxField {
                    label: "Label Size"
                    from: 1
                    to: 100
                    Layout.preferredWidth: 200
                    value: App.theme._labelSize
                    onValueModified: {
                        App.theme._labelSize = value
                    }
                }

                STSpinBoxField {
                    label: "Normal Font Size"
                    from: 1
                    to: 100
                    Layout.preferredWidth: 200
                    value: App.theme._normalSize
                    onValueModified: {
                        App.theme._normalSize = value
                    }
                }

                SliderField {
                    Layout.preferredWidth: 250
                    Layout.maximumWidth: 350

                    from: 0
                    to: 200
                    value: App.theme.zoomLevel * 100
                    onModified: {
                        App.theme.zoomLevel = value / 100
                    }

                    text: "Zoom Level"
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
