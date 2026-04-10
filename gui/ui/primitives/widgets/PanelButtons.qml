import QtQuick
import QtQuick.Layouts
import SolTrace

RowLayout {
    id: root
    required property PanelData target
    required property PanelData otherTarget
    required property int available_width
    required property bool is_right_panel

    Layout.minimumWidth: implicitWidth
    spacing: 5

    STIconButton {
        id: inline_docs_button

        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitWidth

        text: "\uf05a"
        visible: root.target.size >= PanelData.Normal
        onClicked: root.target.inline_docs = !root.target.inline_docs
    }

    STIconButton {
        id: smaller_button

        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitWidth

        text: "\uf422"
        visible: root.target.size !== PanelData.Small
        onClicked: {
            if (root.target.size == PanelData.Full)
                root.otherTarget.visible = true

            root.target.width = root.target.sizes[root.target.size - 1]
            App.view.fit_panels(root.available_width, root.is_right_panel, false)
        }
    }

    STIconButton {
        id: larger_button

        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitWidth

        text: "\uf424"
        visible: root.target.size < 2
        onClicked: {
            root.target.width = root.target.sizes[root.target.size + 1]
            App.view.fit_panels(root.available_width, root.is_right_panel, false)
        }
    }

    STIconButton {
        id: fullsize

        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitWidth

        text: "\uf065"
        visible: root.target.size !== PanelData.Full
        onClicked: {
            root.target.width = root.target.sizes[PanelData.Full]
            App.view.fit_panels(root.available_width, root.is_right_panel, false)
        }
    }

    STIconButton {
        id: close_button

        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitWidth

        text: "\uf00d"
        onClicked: root.target.visible = false
    }
}
