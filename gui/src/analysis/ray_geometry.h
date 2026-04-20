#pragma once


#include "job_control/job_run_common.h"
#include "utilities/qt_helpers.h"

#include <QtQuick3D/qquick3dgeometry.h>


namespace analysis {

// TODO make all deltas queued up for Concurrent off thread rebuilding of geom
// TODO move to tubes and instancing?
class RayGeometry : public QQuick3DGeometry {
    Q_OBJECT

    db::SimulationResultPtr m_database;

    std::unordered_set<db::RayEventType> m_exclude_events = {
        db::RayEventType::CREATE,
        db::RayEventType::VIRTUAL,
        db::RayEventType::UNKNOWN
    };

    /*
    std::unordered_set<SD::element_id> m_selected_elements;
    std::unordered_set<RD::ray_id>     m_selected_rays;
    */

    Q_WRITABLE_PROPERTY(float, show_percent, 50);


public:
    explicit RayGeometry(QQuick3DObject* parent = nullptr);

    void set_results(db::SimulationResultPtr);

public slots:
    void rebuild_geometry();
};

} // namespace analysis
