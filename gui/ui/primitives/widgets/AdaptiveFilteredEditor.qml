import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import SolTrace

AdaptiveEditor {
    id: root

    property var source_model: null

    property string filterText

    onFilterTextChanged: {
        filtered_model.invalidate()
        clearSelection()
    }

    model: SortFilterProxyModel {
        id: filtered_model
        model: root.source_model

        filters: [
            FunctionFilter {
                id: filter

                component NameFilter: QtObject {
                    property string name
                }

                function filter(data: NameFilter) : bool {
                    if (root.filterText.length) {
                        return (data.name || "").toLowerCase().includes(
                                    root.filterText.toLowerCase()
                                    )
                    }
                    return true
                }
            }
        ]
    }
}
