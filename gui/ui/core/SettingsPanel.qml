import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import SolTrace

ShadowedGlassRectangle {
    id: root
    required property int available_width

    ColumnLayout {

        anchors.fill: parent
        anchors.margins: 8

        RowLayout {

            Item {
                Layout.preferredWidth: close_button.width
            }

            Label {
                Layout.fillWidth: true

                text: "Settings"
                font.bold: true
                font.pointSize: 18
                font.family: "CMU Serif"

                elide: Label.ElideRight
                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignVCenter
            }

            STIconButton {
                id: close_button

                Layout.preferredWidth: implicitWidth
                Layout.preferredHeight: implicitWidth
                text: "\uf00d"
                onClicked: App.view.settings_panel.visible = false
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: App.theme.dividerColor
        }

        AdaptiveEditor {
            id: editor

            Layout.fillWidth: true
            Layout.fillHeight: true

            wideThreshold: 500
            listWidth: 200

            model: ListModel {
                ListElement { name: "Theme"; icon: "\uf53f" }
                ListElement { name: "Documentation"; icon: "\uf02d" }
            }

            listDelegate: STItemDelegate {
                text: itemModel ? itemModel.name : ""
                highlighted: isCurrent

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
            }

            detailView: StackLayout {
                currentIndex: editor.currentIndex

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 12

                        Label {
                            text: "Theme"
                            font.family: "CMU Serif"
                            font.pointSize: 16
                            font.bold: true
                        }

                        STButton {
                            text: "Enable Translucency"
                            text_icon: ""
                        }

                        Item { Layout.fillHeight: true }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 12

                        Label {
                            text: "Documentation"
                            font.family: "CMU Serif"
                            font.pointSize: 16
                            font.bold: true
                        }

                        STComboBar {
                            id: bar

                            currentIndex: App.docs.locale
                            onCurrentIndexChanged: App.docs.locale = currentIndex

                            Layout.fillWidth: true

                            model: ["English", "Spanish"]

                        }

                        Item { Layout.fillHeight: true }
                    }
                }
            }

            placeholder: Item {
                Label {
                    anchors.centerIn: parent
                    text: "Select a section"
                    font.pointSize: 16
                    opacity: 0.5
                }
            }
        }

    }
}
