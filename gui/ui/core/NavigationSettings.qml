import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

STPopup {

    margins: 10

    width: 320

    // Navigation
    ColumnLayout {
        anchors.fill: parent
        InlineDocumentation {
            key: "placeholder_small"
            title: "Cameras"
            target: AppData.view.left_panel
        }

        STComboBar {
            currentIndex: AppData.view.sim.camera
            onCurrentIndexChanged: AppData.view.sim.camera  = currentIndex

            Layout.fillWidth: true

            fontFamily: AppData.view.left_panel.size === PanelData.Small ? "Font Awesome 7 Free" : ""

            model : AppData.view.left_panel.size === PanelData.Small ?
                        ["\uf828", "\uf03d"]
                      :
                        ["FPS Camera", "Orbital Camera"]
        }

        InlineDocumentation {
            key: "placeholder_small"
            title: "Camera Perspectives"
            target: AppData.view.left_panel
        }

        STComboBar {
            currentIndex: AppData.view.sim.perspective
            onCurrentIndexChanged: AppData.view.sim.perspective  = currentIndex

            Layout.fillWidth: true

            fontFamily: AppData.view.left_panel.size === PanelData.Small ? "Font Awesome 7 Free" : ""

            model : AppData.view.left_panel.size === PanelData.Small ?
                        ["\uf06a", "\uf06a"]
                      :
                        ["Perspective", "Orthographic"]
        }

        GridLayout {
            columns: 2

            Layout.fillWidth: true

            uniformCellWidths: true
            uniformCellHeights: true

            Label {
                text: "Align View"
                Layout.columnSpan: 2
            }

            STButton {
                text: "-X"
                onClicked: simulation_scene.align_to_axis(CameraController.Axis.X, true)
            }

            STButton {
                text: "X"
                onClicked: simulation_scene.align_to_axis(CameraController.Axis.X, false)
            }

            STButton {
                text: "-Y"
                onClicked: simulation_scene.align_to_axis(CameraController.Axis.Y, true)
            }

            STButton {
                text: "Y"
                onClicked: simulation_scene.align_to_axis(CameraController.Axis.Y, false)
            }

            STButton {
                text: "-Z"
                onClicked: simulation_scene.align_to_axis(CameraController.Axis.Z, true)
            }

            STButton {
                text: "Z"
                onClicked: simulation_scene.align_to_axis(CameraController.Axis.Z, false)
            }
        }

        CheckBoxField {
            Layout.fillWidth: true

            text: "Blueprint Mode"
            value: App.view.sim.blueprint_mode

            onClicked: {
                App.view.sim.blueprint_mode = !App.view.sim.blueprint_mode
            }
        }
    }

}
