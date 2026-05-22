import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Layouts

import SolTrace

STPopup {
    id: root

    property bool has_new_notifications: false

    onOpened: has_new_notifications = false

    ListModel {
        id: notification_model
    }

    Connections {
        target: AppData

        function onNotification(new_note) {

            if (!root.visible) root.has_new_notifications = true

            console.log("Adding note: ", new_note.message)

            notification_model.insert(
                0,
                {
                    "message": new_note.message,
                    "type" : new_note.type
                }
            )

            while (notification_model.count > 50) {
                notification_model.remove(notification_model.count - 1)
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ListView {
            Layout.preferredHeight: 300
            Layout.preferredWidth: 250
            Layout.fillWidth: true

            model: notification_model

            delegate: RowLayout {
                required property string message
                required property int type
                width: ListView.view.width

                spacing: 8

                Label {
                    fontSizeMode: Label.Fit
                    font.pointSize: 18
                    Layout.preferredWidth: 24

                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter

                    font.family: "Font Awesome 7 Free"

                    text: {
                        switch (type) {
                        case 0: return "\uf05a"
                        case 1: return "\uf071"
                        case 2: return "\uf057"
                        default: return "\uf05a"
                        }
                    }
                }

                Label {
                    Layout.fillWidth: true
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    elide: Label.ElideRight

                    text: message
                }
            }

            Label {
                anchors.centerIn: parent



                enabled: false

                text: "No notifications"

                visible: parent.count === 0
            }
        }
    }

}
