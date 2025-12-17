import QtQuick 2.15
import SolTraceProto

Item {
    id: root
    property var source: null
    property int preferredHeight: 50
    property int iconSize: 22
    property int paddingSize: 25
    property string filePath: "Home/Examples/Input.stinput"
    property int sectionIndex: 0

    width: parent.width
    height: preferredHeight

    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }

    GlassRect {
        id: logoHeader
        width: leftContent.implicitWidth + 40
        height: parent.height
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        source: root.source
        radius: 25

        Row {
            id: leftContent
            anchors.centerIn: parent
            spacing: 0

            Image {
                width: 30
                height: 30
                source: "qrc:/images/assets/images/mark.png"
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectCrop
                smooth: true
                anchors.verticalCenter: parent.verticalCenter
            }

            Item {
                width: 10
                height: parent.height
            }

            Text {
                id: logoText
                text: "SolTrace"
                font.family: "CMU Serif"
                font.pixelSize: 20
                color: "white"
                font.weight: Font.Bold
                font.capitalization: Font.SmallCaps
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 2
            }

            Item {
                width: 25
                height: parent.height
            }

            IconButton {
                label: "Configure"
                source: "qrc:/icons/assets/icons/sliders.svg"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: root.sectionIndex = 0
            }

            Image {
                width: 15
                height: 15
                source: "qrc:/icons/assets/icons/double-chevron-right.svg"
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectCrop
                smooth: true
                anchors.verticalCenter: parent.verticalCenter
            }

            IconButton {
                label: "Simulate"
                source: "qrc:/icons/assets/icons/sun.svg"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: root.sectionIndex = 1
            }

            Image {
                width: 15
                height: 15
                source: "qrc:/icons/assets/icons/double-chevron-right.svg"
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectCrop
                smooth: true
                anchors.verticalCenter: parent.verticalCenter
            }

            IconButton {
                label: "Analyze"
                source: "qrc:/icons/assets/icons/chart.svg"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: root.sectionIndex = 2
            }
        }
    }

    GlassRect {
        id: workspaceHeader
        width: 500
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        source: root.source
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
                        font.pixelSize: parent.isLast ? 18 : 15
                        font.weight: parent.isLast ? Font.Bold : Font.Normal
                        color: "white"
                        opacity: parent.isLast ? 1.0 : 0.8
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: parent.isLast ? 1 : 2
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

    GlassRect {
        id: buttonGroup
        width: rightContent.implicitWidth + paddingSize
        height: parent.height
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        source: root.source
        radius: 25

        Row {
            id: rightContent
            anchors.centerIn: parent
            spacing: 10

            IconButton {
                label: "Github"
                source: "qrc:/icons/assets/icons/github.svg"
                anchors.verticalCenter: parent.verticalCenter
            }

            IconButton {
                label: "Settings"
                source: "qrc:/icons/assets/icons/settings.svg"
                anchors.verticalCenter: parent.verticalCenter
            }

            IconButton {
                label: "Profile"
                source: "qrc:/icons/assets/icons/user.svg"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
