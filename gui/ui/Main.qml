import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs

import QtGraphs
import SolTraceProto

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true
    property var workflow: ["Configure", "Simulate", "Analyze"]

    SimulationScene {
        id: sim
        z: 0
        objectName: "simulationScene"
    }


    Item {
        anchors.fill: parent
        anchors.margins: 15
        z: 1

        TopPane {
            id: topbar
            source: sim
        }

        Loader {
            id: content_loader
            anchors.top: topbar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: bottombar.top
            anchors.topMargin: 15
            anchors.bottomMargin: 15
            onLoaded: item.source = sim
            source: "core/" + root.workflow[Session.core.workflowIndex] + ".qml"
        }

        BottomPane {
            id: bottombar
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
