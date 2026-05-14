import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import SolTrace

ShadowedGlassRectangle {
    id: root
    required property int available_width

    Item {

        anchors.fill: parent
        anchors.margins: 8

        STIconButton {
            id: close_button

            anchors.top: parent.top
            anchors.right: parent.right

            text: "\uf00d"
            onClicked: {
                App.view.settings_panel.visible = false
                App.view.left_panel.visible = App.view.left_panel.
                App.view.right_panel.visible = App.view.right_panel.saved_visible
            }

            z: 3
        }

        AdaptiveEditor {
            id: editor

            anchors.fill: parent

            wideThreshold: 500
            listWidth: 200
            currentIndex: App.view.documentation_section
            onCurrentIndexChanged: {
                App.view.documentation_section = currentIndex
            }

            model: ListModel {
                ListElement { name: "What's New"; icon: "✦" }
                ListElement { name: "Theme"; icon: "\uf53f" }
                ListElement { name: "Documentation"; icon: "\uf02d" }
                ListElement { name: "Team"; icon: "\uf500" }
            }

            listDelegate: ItemDelegate {
                text: itemModel ? itemModel.name : ""
                highlighted: isCurrent
                width: parent ? parent.width : implicitWidth

                contentItem: RowLayout {
                    spacing: 8
                    Label {
                        text: itemModel ? itemModel.icon : ""
                        font.family: "Font Awesome 7 Free"
                        font.pointSize: 14
                    }
                    Label {
                        text: itemModel ? itemModel.name : ""
                        Layout.fillWidth: true
                    }
                }

                background: Rectangle {
                    implicitHeight: 36
                    implicitWidth: 100
                    opacity: enabled ? 1 : 0.3
                    color: parent.down ? Material.rippleColor
                         : parent.highlighted ? Qt.rgba(Material.accentColor.r,
                                                         Material.accentColor.g,
                                                         Material.accentColor.b, 0.12)
                         : "transparent"
                    radius: 14
                }
            }

            detailView: StackLayout {
                currentIndex: editor.currentIndex

                ScrollView {
                    id: newScroll
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: availableWidth

                    ColumnLayout {
                        width: newScroll.availableWidth
                        spacing: 12

                        ListModel {
                            id: backendFeatureModel
                            ListElement {
                                name: "GPU Ray Tracing"
                                icon: "\uf2db"
                                description: "Massively parallel Monte Carlo ray tracing on GPU architecture. Full physical fidelity with order-of-magnitude speedups over legacy CPU-bound workflows."
                            }
                            ListElement {
                                name: "Large-Scale Simulations"
                                icon: "\uf0b2"
                                description: "Robust handling of high-resolution, full-scale heliostat fields. Simulations that were previously impractical are now routine."
                            }
                            ListElement {
                                name: "Open-Source & Extensible"
                                icon: "\uf121"
                                description: "Modular codebase with clear APIs. Customizable beyond proprietary software limits, lowering barriers to entry for researchers and developers."
                            }
                            ListElement {
                                name: "SAM Integration"
                                icon: "\uf0c1"
                                description: "Coupling optical simulations with system-level techno-economic models like SAM for end-to-end CST performance and cost analysis."
                            }
                            ListElement {
                                name: "Python API"
                                icon: "\uf120"
                                description: "Scriptable interface via pysoltrace for parametric studies, optimization loops, and integration with existing Python-based research pipelines."
                            }
                        }

                        ListModel {
                            id: guiFeatureModel
                            ListElement {
                                name: "3D Viewport"
                                icon: "\uf1b2"
                                description: "Interactive 3D visualization of optical geometries, ray intersections, and flux maps. Explore your system spatially instead of through tables."
                            }
                            ListElement {
                                name: "Geometry Editor"
                                icon: "\uf5ee"
                                description: "Direct manipulation of stages, elements, and optical properties. Eliminates trial-and-error geometry generation from legacy workflows."
                            }
                            ListElement {
                                name: "Flux Visualization"
                                icon: "\uf06d"
                                description: "Real-time flux map rendering and scatter plots. Quickly assess optical performance without exporting data to external tools."
                            }
                            ListElement {
                                name: "Theming & Accessibility"
                                icon: "\uf53f"
                                description: "Customizable glass UI with adjustable colors, font sizes, and zoom levels. Adapts to user preferences and display environments."
                            }
                            ListElement {
                                name: "Documentation"
                                icon: "\uf02d"
                                description: "Built-in multilingual documentation and help resources. Minimizes learning curve and provides guidance without leaving the application."
                            }
                        }

                        Header {
                            text: "New in SolTrace"
                        }

                        STPropertyPanel {
                            title: "Backend Features"
                            Layout.fillWidth: true
                            CardGallery {
                                model: backendFeatureModel
                                Layout.fillWidth: true
                                Layout.columnSpan: 2
                                delegate: ColumnLayout {
                                    property string name
                                    property string icon
                                    property string description
                                    spacing: 5
                                    RowLayout {
                                        spacing: 8
                                        STLabel {
                                            text: icon
                                            font.family: "Font Awesome 7 Free"
                                            font.pointSize: 16
                                        }
                                        SubHeader { text: name }
                                    }
                                    STLabel {
                                        Layout.fillWidth: true
                                        text: description
                                        wrapMode: Text.WordWrap
                                    }
                                }
                            }
                        }

                        STPropertyPanel {
                            title: "GUI Features"
                            Layout.fillWidth: true
                            CardGallery {
                                model: guiFeatureModel
                                Layout.fillWidth: true
                                Layout.columnSpan: 2
                                delegate: ColumnLayout {
                                    property string name
                                    property string icon
                                    property string description
                                    spacing: 5
                                    RowLayout {
                                        spacing: 8
                                        STLabel {
                                            text: icon
                                            font.family: "Font Awesome 7 Free"
                                            font.pointSize: 16
                                        }
                                        SubHeader { text: name }
                                    }
                                    STLabel {
                                        Layout.fillWidth: true
                                        text: description
                                        wrapMode: Text.WordWrap
                                    }
                                }
                            }
                        }
                    }
                }

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

                ScrollView {
                    id: docScroll
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: availableWidth

                    ColumnLayout {
                        width: docScroll.availableWidth
                        spacing: 12

                        Header {
                            text: "Documentation"
                        }

                        STPropertyPanel {
                            Layout.fillWidth: true

                            title: "GitHub"
                            collapsed: false

                            GridLayout {
                                rows: 2
                                columns: 2

                                STButton {
                                    text: "SolTrace"
                                    text_icon: "\uf08e"

                                    onClicked: {
                                        Qt.openUrlExternally("https://github.com/NatLabRockies/SolTrace")
                                    }
                                }

                                STButton {
                                    text: "SolTrace Issue Tracker"
                                    text_icon: "\uf08e"

                                    onClicked: {
                                        Qt.openUrlExternally("https://github.com/NatLabRockies/SolTrace/issues")
                                    }
                                }

                                STButton {
                                    text: "SolTrace GUI"
                                    text_icon: "\uf08e"

                                    onClicked: {
                                        Qt.openUrlExternally("https://github.com/nicholasbl/SolTrace")
                                    }
                                }

                                STButton {
                                    text: "SolTrace GUI Issue Tracker"
                                    text_icon: "\uf08e"

                                    onClicked: {
                                        Qt.openUrlExternally("https://github.com/nicholasbl/SolTrace/issues")
                                    }
                                }
                            }
                        }

                        STPropertyPanel {
                            Layout.fillWidth: true

                            title: "Inline Documentation"
                            collapsed: false

                            ColumnLayout {
                                STComboBox {
                                    currentIndex: App.docs.locale
                                    model: ["English", "Spanish"]
                                    onCurrentIndexChanged: App.docs.locale = currentIndex
                                }
                            }
                        }

                        Item { Layout.fillHeight: true }
                    }
                }

                ScrollView {
                    id: teamScroll
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: availableWidth

                    ColumnLayout {
                        width: teamScroll.availableWidth
                        spacing: 12

                        Header {
                            text: "SolTrace Team"
                        }

                        ListModel {
                            id: frontendTeamModel

                            ListElement {
                                name: "Nicholas Brunhart-Lupo"
                                role: "Lorem Ipsum"
                                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                                url: "Test"
                                email: "@nrel.gov"
                            }

                            ListElement {
                                name: "Rianna Shantivong"
                                role: "Lorem Ipsum"
                                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                                url: "Test"
                                email: "@nrel.gov"
                            }

                            ListElement {
                                name: "Kenny Gruchalla"
                                role: "Lorem Ipsum"
                                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                                url: "Test"
                                email: "@nrel.gov"
                            }

                        }

                        ListModel {
                            id: backendTeamModel
                            ListElement {
                                name: "Michael Wagner"
                                role: "Lorem Ipsum"
                                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                                website: "Test"
                                email: "@nrel.gov"
                            }

                            ListElement {
                                name: "Taylor Brown"
                                role: "Lorem Ipsum"
                                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                                website: "Test"
                                email: "@nrel.gov"
                            }

                            ListElement {
                                name: "Jonathan Maack"
                                role: "Lorem Ipsum"
                                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                                website: "Test"
                                email: "@nrel.gov"
                            }

                            ListElement {
                                name: "Luning Fang"
                                role: "Lorem Ipsum"
                                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                                website: "Test"
                                email: "@nrel.gov"
                            }

                            ListElement {
                                name: "Nick Edwards"
                                role: "Lorem Ipsum"
                                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                                website: "Test"
                                email: "@nrel.gov"
                            }
                        }

                        TeamGallery {
                            title: "Principal Investigator"
                            Layout.fillWidth: true
                            Layout.columnSpan: 2

                            model: ListModel {
                                ListElement {
                                    name: "William Hamilton"
                                    role: "Principal Investigator"
                                    description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat."
                                    website: "Test"
                                    email: "@nrel.gov"
                                }
                            }
                        }

                        TeamGallery {
                            title: "Backend Team"
                            Layout.fillWidth: true
                            Layout.columnSpan: 2

                            model: backendTeamModel
                        }

                        TeamGallery {
                            title: "Frontend Team"
                            Layout.fillWidth: true
                            Layout.columnSpan: 2

                            model: frontendTeamModel
                        }

                        Item { Layout.fillHeight: true }
                    }
                }

            }
        }
    }
}
