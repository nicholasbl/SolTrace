import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

ScrollView {
    id: teamScroll
    Layout.fillWidth: true
    Layout.fillHeight: true
    contentWidth: availableWidth

    ColumnLayout {
        width: teamScroll.availableWidth
        spacing: 12

        Header {
            text: "SolTrace Team"
        }

        ListModel {
            id: frontendTeamModel

            ListElement {
                name: "Nicholas Brunhart-Lupo"
                role: "Lorem Ipsum"
                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                url: "Test"
                email: "@nrel.gov"
            }

            ListElement {
                name: "Rianna Shantivong"
                role: "Lorem Ipsum"
                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                url: "Test"
                email: "@nrel.gov"
            }

            ListElement {
                name: "Kenny Gruchalla"
                role: "Lorem Ipsum"
                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                url: "Test"
                email: "@nrel.gov"
            }

        }

        ListModel {
            id: backendTeamModel
            ListElement {
                name: "Michael Wagner"
                role: "Lorem Ipsum"
                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                website: "Test"
                email: "@nrel.gov"
            }

            ListElement {
                name: "Taylor Brown"
                role: "Lorem Ipsum"
                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                website: "Test"
                email: "@nrel.gov"
            }

            ListElement {
                name: "Jonathan Maack"
                role: "Lorem Ipsum"
                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                website: "Test"
                email: "@nrel.gov"
            }

            ListElement {
                name: "Luning Fang"
                role: "Lorem Ipsum"
                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                website: "Test"
                email: "@nrel.gov"
            }

            ListElement {
                name: "Nick Edwards"
                role: "Lorem Ipsum"
                description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
                website: "Test"
                email: "@nrel.gov"
            }
        }

        TeamGallery {
            title: "Principal Investigator"
            Layout.fillWidth: true
            Layout.columnSpan: 2

            model: ListModel {
                ListElement {
                    name: "William Hamilton"
                    role: "Principal Investigator"
                    description: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat."
                    website: "Test"
                    email: "@nrel.gov"
                }
            }
        }

        TeamGallery {
            title: "Backend Team"
            Layout.fillWidth: true
            Layout.columnSpan: 2

            model: backendTeamModel
        }

        TeamGallery {
            title: "Frontend Team"
            Layout.fillWidth: true
            Layout.columnSpan: 2

            model: frontendTeamModel
        }

        Item { Layout.fillHeight: true }
    }
}
