pragma Singleton

import QtQuick
import QtCore
import SolTrace

QtObject {
    id: root

    readonly property Theme theme: Theme {}
    // readonly property Settings settings: Settings {}

    readonly property var db: data.current_database
    readonly property var file_source: data.file_source
    readonly property var sun: data.sun
    readonly property var layout: data.layout
    readonly property var view: data.view
    readonly property var materials: data.materials
    readonly property var tracing: data.tracing
    readonly property var simulation: data.simulation
    readonly property var docs: data.docs
}
