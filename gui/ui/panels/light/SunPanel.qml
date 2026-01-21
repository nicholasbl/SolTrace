import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto

Column {
    id: root
    anchors.fill: parent
    anchors.margins: 20
    anchors.rightMargin: 50
    spacing: 10

    DocHeaderSection {
        width: parent.width
        header.text: Documentation.sun.headers.section
        body.text: Documentation.sun.intro
    }

    DocSection {
        width: parent.width
        header.text: Documentation.sun.headers.sunTypes
        body.text: Documentation.sun.sunTypes
    }

    GlassTabBar {
        model: ["Directional Sun", "Point Source Sun"]
        index: Session.core.sun.sunTypeIndex
        onIndexRequested: (newIndex) =>  Session.core.sun.sunTypeIndex = newIndex
    }

    Spacer {visible: Settings.showDocumentation}

    Item {
        width: parent.width
        height: childrenRect.height
        DirectionalSunPanel {
            id: directionalPanel
            width: parent.width
            visible: Session.core.sun.sunTypeIndex === 0
            height: visible ? implicitHeight : 0
        }

        PointSourceSunPanel {
            id: pointSourcePanel
            width: parent.width
            visible: Session.core.sun.sunTypeIndex === 1
            height: visible ? implicitHeight : 0
        }
    }

    DocSection {
        width: parent.width
        header.text: Documentation.sun.headers.sunShapes
        body.text: Documentation.sun.sunShapes
    }

    GlassTabBar {
        model: ["Gaussian", "Pillbox", "Buie CSR", "Custom"]
        index: Session.core.sun.sunShapeIndex
        onIndexRequested: (newIndex) => Session.core.sun.sunShapeIndex = newIndex
    }

    Spacer {visible: Settings.showDocumentation}

    SunShapePanel {
        width: parent.width
    }

    Item {
        width: 1
        height: 100
    }
}
