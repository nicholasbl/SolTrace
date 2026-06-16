import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import SolTrace

STMenu {
    id: root

    required property var controller

    MenuItem {
        text: "New"
        enabled: !AppData.file_source.is_loading
        onClicked: root.controller.load_new()
    }

    MenuItem {
        text: "Open"
        enabled: !AppData.file_source.is_loading
        onClicked: root.controller.open_file()
    }

    STMenu {
        id: recents_menu

        title: qsTr("Recent Files")
        enabled: recent_instantiator.count > 0

        Instantiator {
            id: recent_instantiator

            model: root.controller.recent_files

            delegate: MenuItem {
                text: root.controller.file_name(modelData)
                onTriggered: {
                    root.dismiss()
                    root.controller.load_recent(modelData)
                }
            }

            onObjectAdded: (index, object) => recents_menu.insertItem(index, object)
            onObjectRemoved: (index, object) => recents_menu.removeItem(object)
        }

        MenuSeparator {
            visible: recent_instantiator.count > 0
        }

        MenuItem {
            text: qsTr("Clear Menu")
            onTriggered: root.controller.clear_recent_files()
        }
    }

    MenuItem {
        text: "Save"
        enabled: !AppData.file_source.is_loading
    }
}
