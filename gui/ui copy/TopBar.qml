import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

TransparentPane {
    background_opacity: 0.5

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        ColumnLayout {
            Layout.fillHeight: true
            Button {
                text: "New"
                Material.elevation: 1
            }

            Button {
                text: "Load"
            }
        }

        Button {
            text: "Design"
        }

        Button {
            text: "Analyze"
        }
    }
}
