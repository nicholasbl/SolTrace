import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import SolTrace

ColumnLayout {
    spacing: 8

    Label {
        Layout.fillWidth: true
        Layout.leftMargin: 10
        Layout.rightMargin: 10
        Layout.bottomMargin: 8

        text: "Add a scene to the environment by loading a .stinput file"
        wrapMode: Text.WordWrap
    }

    SceneListPane {}
}
