import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs

import SolTrace

ApplicationWindow {
    id: main_window
    width: 1280
    height: 720
    visible: true

    minimumWidth: 600
    minimumHeight: 600

    Material.theme: Material.Dark
    Material.accent: Material.Blue

    font.pointSize: 12
    font.family: "Roboto"

    Component.onCompleted: {
        App.file_source.source = "file:///Users/rsantivo/Documents/Coding/SolTrace/SolTrace/gui/assets/examples/Power-tower-surround.stinput"
    }

    SimulationScene {
        id: simulation_scene

        anchors.fill: parent
    }

    CorePanel {
        blur_source: simulation_scene
        anchors.fill: parent
    }
}
