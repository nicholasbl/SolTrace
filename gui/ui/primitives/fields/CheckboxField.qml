import QtQuick
import SolTrace

STButton {
    property bool value: false

    text_icon: value ? "\uf14a" : "\uf0c8"
}
