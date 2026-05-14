import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import SolTraceProto
ColumnLayout {
    id: root

    spacing: 10
    DocHeaderSection {
        width: parent.width
        header.text: "Layout"
        body.text: Documentation.staging.staging
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10

            Item {
                id: left_side
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 48

                        Repeater {
                            model: App.layout.breadcrumb_model

                            delegate: STLabel {
                                text: ">" + name
                            }
                        }
                    }

                    Rectangle {
                        color: "red"
                        Layout.preferredHeight: 1
                        Layout.fillWidth: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        model: App.layout.child_model

                        delegate: Item {
                            width: ListView.view.width
                            height: 48

                            required property string name
                            required property var entity_id

                        }
                    }
                }
            }

            Item {
                id: right_side
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1

                ScrollView {
                    id: scroll_view
                    anchors.fill: parent
                    contentWidth: availableWidth


                    ColumnLayout {
                        id: panel

                        width: scroll_view.availableWidth

                        PropertyPanel {
                            Layout.fillWidth: true

                            PropertyLabel {
                                text: "Name:"
                            }

                            GlassTextField {
                                Layout.fillWidth: true
                            }

                            PropertyLabel {
                                text: "Visible:"
                            }

                            CheckBox {
                                Layout.fillWidth: true
                            }

                            PropertyLabel {
                                text: "Position:"
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                GlassTextField {
                                    Layout.fillWidth: true
                                }

                                GlassTextField {
                                    Layout.fillWidth: true
                                }

                                GlassTextField {
                                    Layout.fillWidth: true
                                }
                            }

                            PropertyLabel {
                                text: "Orientation:"
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                GlassTextField {
                                    Layout.fillWidth: true
                                }

                                GlassTextField {
                                    Layout.fillWidth: true
                                }

                                GlassTextField {
                                    Layout.fillWidth: true
                                }
                            }

                            PropertyLabel {
                                text: "Material Group:"
                            }

                            GlassComboBox {
                                Layout.fillWidth: true
                            }

                            PropertyLabel {
                                text: "Parent"
                            }

                            GlassComboBox {
                                Layout.fillWidth: true
                            }


                        }

                    }

                }
            }

        }
    }

}
