import QtQuick
import QtQuick.Layouts
import SolTrace

ColumnLayout {
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    WorkflowStepper {
        Layout.fillWidth: true
        next: "Load Scene"
        currentIndex: ViewModule.Start
    }
}
