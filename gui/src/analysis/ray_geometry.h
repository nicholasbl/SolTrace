#pragma once


#include "job_control/job_run_common.h"
#include "utilities/qt_helpers.h"

#include <QtQuick3D/qquick3dgeometry.h>


namespace analysis {

struct EventTypeContainer {
    std::unordered_set<db::RayEventType> events;

    EventTypeContainer() = default;

    EventTypeContainer(std::initializer_list<db::RayEventType>);
    EventTypeContainer(QStringList);

    QStringList to_list() const;
};

// TODO make all deltas queued up for Concurrent off thread rebuilding of geom
// TODO move to tubes and instancing?
class RayGeometry : public QQuick3DGeometry {
    Q_OBJECT

    db::SimulationResultPtr m_database;

    EventTypeContainer m_include_events;

    /*
    std::unordered_set<SD::element_id> m_selected_elements;
    std::unordered_set<RD::ray_id>     m_selected_rays;
    */

    Q_WRITABLE_PROPERTY(QStringList, event_include, {});
    Q_WRITABLE_PROPERTY(float, show_percent, 100);
    Q_READONLY_PROPERTY(quint64, available_rays);

private slots:
    void inclusion_list_update();

public:
    explicit RayGeometry(QQuick3DObject* parent = nullptr);

    void set_results(db::SimulationResultPtr);

public slots:
    void rebuild_geometry();
};

} // namespace analysis
