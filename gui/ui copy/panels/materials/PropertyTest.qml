import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QC

Item {
    QC.ScrollView {
        id: scroll_view
        anchors.fill: parent
        contentWidth: availableWidth
        ColumnLayout {
            id: panel

            width: scroll_view.availableWidth

            PropertyPanel {
                Layout.fillWidth: true
                title: "Hello"

                PropertyLabel {
                    text: "Item 1:"
                }

                QC.ComboBox {
                    model: 5

                    Layout.fillWidth: true
                }


                PropertyLabel {
                    text: "Item 2:"
                }

                Rectangle {
                    color: "blue"

                    Layout.fillWidth: true
                }

                PropertyLabel {
                    text: "Very long Item 2:"
                }

                QC.Slider {
                    from: 0
                    to: 100

                    Layout.fillWidth: true
                }

                PropertyPanel {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    title: "Hello"

                    PropertyLabel {
                        text: "Item 1:"
                    }

                    QC.ComboBox {
                        model: 5

                        Layout.fillWidth: true
                    }


                    PropertyLabel {
                        text: "Item 2:"
                    }

                    Rectangle {
                        color: "blue"

                        Layout.fillWidth: true
                    }
                }
            }

            PropertyPanel {
                Layout.fillWidth: true
                title: "There"

                PropertyLabel {
                    text: "Item 1:"
                }

                QC.ComboBox {
                    model: 5

                    Layout.fillWidth: true
                }


                PropertyLabel {
                    text: "Item 2:"
                }

                Rectangle {
                    color: "blue"

                    Layout.fillWidth: true
                }

                PropertyLabel {
                    text: "Very long Item 2:"
                }

                QC.Slider {
                    from: 0
                    to: 100

                    Layout.fillWidth: true
                }

                PropertyLinearCombo {

                    Layout.columnSpan: 2

                    model: ["Do A", "Do B", "Do C"]
                    currentIndex: 1
                }

            }
        }
    }
}


