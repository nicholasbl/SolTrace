import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import SolTraceProto

Item {
    id: root
    property var source: null
    property int preferredHeight: 35
    property int iconSize: 22
    property int paddingSize: 25
    property string filePath: "Home/Examples/Input.stinput"
    property int iconSpacing: 4

    width: parent.width
    height: preferredHeight

    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }

    Row {
        id: leftGroup
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10

        GlassRectangle {
            id: logoHeader
            width: leftContent.implicitWidth + 40
            height: root.preferredHeight
            anchors.verticalCenter: parent.verticalCenter
            radius: 25

            Row {
                id: leftContent
                anchors.centerIn: parent
                spacing: 0

                Item {
                    width: logoRow.width + 4
                    height: logoRow.height + 1

                    Row {
                        id: logoRow
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.topMargin: 0
                        spacing: 2

                        Image {
                            width: 35
                            height: 35
                            source: "qrc:/images/assets/images/logo.svg"
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                            smooth: true
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Label {
                            id: logoText
                            text: "SolTrace"
                            font.family: "CMU Serif"
                            font.pixelSize: 18
                            color: "white"
                            font.weight: Font.Bold
                            font.capitalization: Font.SmallCaps
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.verticalCenterOffset: 2
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: "white"
                        anchors.top: logoRow.bottom
                        anchors.topMargin: -7
                    }
                }
            }
        }

        GlassRectangle {
            id: fileActionsHeader
            width: fileActions.implicitWidth + paddingSize
            height: root.preferredHeight
            anchors.verticalCenter: parent.verticalCenter
            radius: 25

            Row {
                id: fileActions
                anchors.centerIn: parent
                spacing: root.iconSpacing

                IconButton {
                    label: "New"
                    source: "qrc:/icons/assets/icons/placeholder.svg"
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: App.file_source.source = ""
                }

                IconButton {
                    label: "Open"
                    source: "qrc:/icons/assets/icons/placeholder.svg"
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: openFileDialog.open()

                    FileDialog {
                        id: openFileDialog
                        currentFolder: StandardPaths.standardLocations(
                                           StandardPaths.DocumentsLocation
                                           )[0]
                        onAccepted: App.file_source.source = selectedFile
                    }
                }

                IconButton {
                    label: "Save"
                    source: "qrc:/icons/assets/icons/save.svg"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    GlassRectangle {
        id: workspaceHeader
        width: 450
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        radius: 25

        Row {
            id: middleContent
            anchors.centerIn: parent
            spacing: 5

            Repeater {
                model: root.filePath.split("/")

                Row {
                    required property string modelData
                    required property int index
                    spacing: 5

                    readonly property bool isLast: index === root.filePath.split("/").length - 1

                    Text {
                        text: modelData
                        font.family: parent.isLast ? "CMU Serif" : ""
                        font.pixelSize: parent.isLast ? 15 : 15
                        font.weight: parent.isLast ? Font.Bold : Font.Normal
                        color: "white"
                        opacity: parent.isLast ? 1.0 : 0.8
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: parent.isLast ? 3 : 2
                    }

                    Text {
                        text: "/"
                        font.family: "CMU Serif"
                        font.pixelSize: 14
                        color: "white"
                        opacity: 0.4
                        visible: !parent.isLast
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 2
                    }
                }
            }
        }
    }

    Row {
        id: rightGroup
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10

        GlassRectangle {
            id: helpGroup
            width: helpContent.implicitWidth + paddingSize
            height: root.preferredHeight
            anchors.verticalCenter: parent.verticalCenter
            source: root.source
            radius: 25

            Row {
                id: helpContent
                anchors.centerIn: parent
                spacing: root.iconSpacing

                IconButton {
                    label: "Docs" + (UserSettings.showDocumentation ? " ▾" : " ▸")
                    source: "qrc:/icons/assets/icons/info.svg"
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: UserSettings.showDocumentation = !UserSettings.showDocumentation
                }

                IconButton {
                    label: "Github"
                    source: "qrc:/icons/assets/icons/github.svg"
                    anchors.verticalCenter: parent.verticalCenter
                }

                IconButton {
                    label: "Help"
                    source: "qrc:/icons/assets/icons/placeholder.svg"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        GlassRectangle {
            id: buttonGroup
            width: rightContent.implicitWidth + paddingSize
            height: root.preferredHeight
            anchors.verticalCenter: parent.verticalCenter
            source: root.source
            radius: 25

            Row {
                id: rightContent
                anchors.centerIn: parent
                spacing: root.iconSpacing

                IconButton {
                    label: "Notifications"
                    source: "qrc:/icons/assets/icons/placeholder.svg"
                    anchors.verticalCenter: parent.verticalCenter
                }

                IconButton {
                    label: "Settings"
                    source: "qrc:/icons/assets/icons/settings.svg"
                    anchors.verticalCenter: parent.verticalCenter
                }

                /* IconButton {
                    label: "Account"
                    source: "qrc:/icons/assets/icons/user.svg"
                    anchors.verticalCenter: parent.verticalCenter
                } */
            }
        }
    }
}
