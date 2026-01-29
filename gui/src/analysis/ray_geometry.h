#pragma once


#include "job_control/job_run_common.h"
#include "qt_helpers.h"
#include "ray_volume.h"

#include <QtQuick3D/qquick3dgeometry.h>


namespace analysis {

// TODO make all deltas queued up for Concurrent off thread rebuilding of geom
// TODO move to tubes and instancing?
class RayGeometry : public QQuick3DGeometry {
    Q_OBJECT

    std::shared_ptr<ResultDB> m_database;

    std::unordered_set<SolTrace::Result::RayEvent> m_exclude_events = {
        SolTrace::Result::RayEvent::CREATE,
        SolTrace::Result::RayEvent::VIRTUAL,
        SolTrace::Result::RayEvent::UNKNOWN
    };

    /*
    std::unordered_set<SD::element_id> m_selected_elements;
    std::unordered_set<RD::ray_id>     m_selected_rays;
    */

    Q_WRITABLE_PROPERTY(float, show_percent, 50);

    QOBJECT_WRITABLE_PROPERTY(RayVolume, ray_volume);


public:
    explicit RayGeometry(QQuick3DObject* parent = nullptr);

    void set_results(std::shared_ptr<ResultDB>);

public slots:
    void rebuild_geometry();
};

} // namespace analysis
