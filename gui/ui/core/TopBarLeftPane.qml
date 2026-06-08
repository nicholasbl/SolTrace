import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

RowLayout {
    id: root

    required property int available_width
    required property bool show_logos

    spacing: 0

    STIconButton {
        id: leftpanel_open
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight
        Layout.leftMargin: 20
        Layout.rightMargin: 10

        text: "\uf0c9"
        toolTip: (App.view.left_panel.visible ? "Close": "Open") + " Left Panel"

        label.font.pointSize: 20
        onClicked: {
            if (App.view.settings_panel.visible) {
                App.view.settings_panel.visible = false
                return
            }

            App.view.left_panel.visible = !App.view.left_panel.visible && !App.view.settings_panel.visible
            App.view.fit_panels(root.available_width)
        }
    }

    Item {
        id: soltrace_logo
        visible: root.show_logos

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter

        implicitWidth: logo_content.implicitWidth
        implicitHeight: logo_content.implicitHeight

        Rectangle {
            anchors.fill: logo_content
            anchors.margins: -5
            radius: 5

            color: logo_mouse_area.containsMouse ? Material.rippleColor :
                                                   "transparent"

            Behavior on color {
                ColorAnimation {
                    duration: 100
                }
            }
        }

        Column {
            id: logo_content
            spacing: -2

            RowLayout {
                id: logo_row
                spacing: 4

                Item { Layout.preferredWidth: 2 }

                Image {
                    id: logo_icon
                    source: "qrc:/assets/images/logo.svg"
                    Layout.preferredHeight: 27
                    Layout.preferredWidth: Layout.preferredHeight
                    Layout.alignment: Qt.AlignBottom
                    mipmap: true
                    fillMode: Image.PreserveAspectFit

                    layer.enabled: true
                    layer.effect: MultiEffect {
                        colorization: 1.0
                        colorizationColor: App.theme.fontColor
                    }
                }

                Label {
                    id: logo_text
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignBottom
                    font.pointSize: 18
                    font.family: "CMU Serif"
                    text: "SolTrace"
                    font.bold: true
                    font.capitalization: Font.SmallCaps
                }

                Item { Layout.preferredWidth: 2 }
            }

            Rectangle {
                color: App.theme.fontColor
                width: logo_row.width
                height: 1
            }
        }

        MouseArea {
            id: logo_mouse_area
            anchors.fill: logo_content
            hoverEnabled: true
            onClicked: version_pop.open()
        }

        STPopup {
            id: version_pop

            contentWidth: 280

            ColumnLayout {
                spacing: 6

                Label {
                    Layout.fillWidth: true

                    text: "SolTrace"
                    font.bold: true
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    Layout.fillWidth: true

                    text: AppData.current_build_info
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                }
            }
        }
    }

    STClickableLabel {
        id: prerel_logo

        visible: root.show_logos && AppData.is_prerelease

        Layout.alignment: Qt.AlignVCenter
        Layout.topMargin: 10
        Layout.rightMargin: 10

        text: "BETA"
        font.pointSize: 10
        font.family: "CMU Serif"
        font.bold: true
        font.italic: true

        color: Material.color(Material.Yellow)

        onClicked: beta_pop.open()

        STPopup {
            id: beta_pop

            contentWidth: 280

            ColumnLayout {
                anchors.fill: parent
                spacing: 6

                Label {
                    Layout.fillWidth: true

                    text: "Pre-release build"
                    font.bold: true
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                }

                Label {
                    Layout.fillWidth: true

                    text: "This version of SolTrace is intended for testing and evaluation. Features, file formats, and simulation behavior may change before the final release. Verify important results with a stable release before using them for production work."
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                }
            }
        }
    }

}
