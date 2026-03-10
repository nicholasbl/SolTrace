import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls.Material
import SolTraceProto

Item {
    id: sidebarBody
    Column {
        id: configurationPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        
        Rectangle {
            width: parent.width
            height: 1
            color: Theme.lineColor
        }
        
        Spacer { height: 10 }
        
        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 20
            spacing: 0
            
            RowLayout {
                width: parent.width
                spacing: 0
                
                Body {
                    text: root.panels[Session.core.sectionIndex]
                    font.family: "CMU Serif"
                    font.bold: true
                }
                
                Item {
                    Layout.fillWidth: true
                }
                
                Row {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.topMargin: -2
                    
                    IconButton {
                        tooltip: "New"
                        iconSize: 18
                        source: "qrc:/icons/assets/icons/placeholder.svg"
                        onClicked: {
                            // Add action
                        }
                    }
                    
                    IconButton {
                        tooltip: "Save"
                        iconSize: 18
                        source: "qrc:/icons/assets/icons/save.svg"
                        onClicked: {
                            // Add action
                        }
                    }
                    
                    IconButton {
                        tooltip: "Import"
                        iconSize: 18
                        source: "qrc:/icons/assets/icons/download.svg"
                        onClicked: {
                            // Add action
                        }
                    }
                    
                    IconButton {
                        tooltip: "Export"
                        iconSize: 18
                        source: "qrc:/icons/assets/icons/upload.svg"
                        onClicked: {
                            // Add action
                        }
                    }
                }
            }
            
            GlassComboBox {
                model: ["Current Config (Unsaved)", "Other Config (Last Run)"]
                width: parent.width
                //height: 30
                //labelFontSize: Theme.textSizeSmall
                ////borderColor: Theme.lineColor
            }
        }
        
        Spacer { height: 20 }
        
    }

    // Move to a stack layout or some such
    Item {
        anchors.top: parent.top
        anchors.bottom: configurationPanel.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20

        ListView {
            anchors.fill: parent
            clip: true
            model: App.materials.materials_list
            delegate: LabeledListItem {
                required property string name
                required property db_entity entity
                text: name

                onClicked: {
                    App.materials.current_material = entity
                }
            }
        }
    }


    
    // ScrollView {
    //     width: parent.width
    //     anchors.top: parent.top
    //     anchors.bottom: configurationPanel.top
    //     clip: true
        
    //     ColumnLayout {
    //         anchors.fill: parent
    //         anchors.margins: 20
    //         width: parent.width
    //         spacing: 10
            
    //         Row {
    //             visible: false
    //             IconButton {
    //                 label: "Collapse All"
    //             }
                
    //             IconButton {
    //                 label: "Expand All"
    //             }
    //         }
            
    //         // TreeNavigator {
    //         //     id: sidebar_tree
    //         //     model: {
    //         //         switch(Session.core.sectionIndex) {
    //         //             case 0: // Sun
    //         //                 return [
    //         //                     {
    //         //                         title: "Sun Type & Direction",
    //         //                         childrenAction: (index) => Session.core.sun.sunTypeIndex = index,
    //         //                         children: [
    //         //                             {
    //         //                                 title: "Directional Sun",
    //         //                                 childrenAction: (index) => Session.core.sun.calculatorIndex = index,
    //         //                                 children: [
    //         //                                     { title: "Legacy Calculator" },
    //         //                                     { title: "Duffie and Beckman" },
    //         //                                     { title: "SOLPOS" },
    //         //                                     { title: "SPA" }
    //         //                                 ]
    //         //                             },
    //         //                             { title: "Point Source Sun" }
    //         //                         ]
    //         //                     },
    //         //                     {
    //         //                         title: "Sun Shape",
    //         //                         childrenAction: (index) => Session.core.sun.sunShapeIndex = index,
    //         //                         children: [
    //         //                             { title: "Gaussian" },
    //         //                             { title: "Pillbox" },
    //         //                             { title: "Buie CSR" },
    //         //                             { title: "Custom" }
    //         //                         ]
    //         //                     }
    //         //                 ]
    //         //             case 1: // Materials
    //         //                 return null
    //         //             case 2: // Staging
    //         //                 return [
    //         //                     { title: "Geometry" },
    //         //                     { title: "Positioning" }
    //         //                 ]
    //         //             case 3: // Tracing
    //         //                 return [
    //         //                     { title: "Ray Tracer" },
    //         //                     { title: "Engine Parameters" },
    //         //                     { title: "Execution Parameters" },
    //         //                     { title: "Optimizations & Errors" }
    //         //                 ]
    //         //             default:
    //         //                 return []
    //         //         }
    //         //     }
    //         //}
            

    //     }
    // }
}
