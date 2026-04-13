import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import SolTrace

ListView {
    id: root
    width: 300
    height: 400
    clip: true

    model: App.sun.shape.custom_distribution

    header: RowLayout {
        width: root.width

        Label {
            text: "Angle (mrad)"
            Layout.fillWidth: true
            font.bold: true
            font.pointSize: 13
        }

        Label {
            text: "Intensity"
            Layout.fillWidth: true
            font.bold: true
            font.pointSize: 13
        }
    }

    delegate: RowLayout {
        width: root.width
        spacing: 4

        Label {
            id: index_label

            Layout.preferredWidth: implicitWidth + 5
            Layout.preferredHeight: implicitWidth + 5
            text: index + 1
        }

        STDoubleSpinBox {
            id: angle

            decimals: 3
            Layout.fillWidth: true
            value: root.model.data(root.model.index(index, 0))
            onValueModified: root.model.setData(root.model.index(index, 0), value, Qt.EditRole)
        }

        STDoubleSpinBox {
            id: intensity

            from: 0
            to: 1.2
            stepSize: 0.1
            decimals: 3
            Layout.fillWidth: true
            value: root.model.data(root.model.index(index, 1))
            onValueModified: root.model.setData(root.model.index(index, 1), value, Qt.EditRole)
        }

        STIconButton {
            id: remove_button

            Layout.preferredWidth: implicitWidth
            Layout.preferredHeight: implicitWidth
            text: "\uf00d"
            onClicked: App.sun.shape.custom_distribution.remove(index)
            iconSize: 10
        }
    }

    footer: STButton {
        width: root.width

        text: "+ Add row"
        onClicked: App.sun.shape.custom_distribution.append(0, 0)
    }
}
