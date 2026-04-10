#pragma once

#include <QImage>
#include <QObject>

#include "baked_flux_map.h"
#include "database/mesh.h"
#include "job_control/job_run_common.h"
#include "utilities/qt_helpers.h"

namespace analysis {

struct FluxMapBakeOptions {
    // TODO: change to support UV aspect ratio
    glm::uvec2 image_resolution = { 1024, 1024 };
    QColor     grid_line_color  = QColor();
    QImage     color_map;
};

class FluxMapComputer : public QObject {
    Q_OBJECT

    db::SimulationResultPtr m_database;

public:
    explicit FluxMapComputer(QObject* parent);
    ~FluxMapComputer() override;

    void set_results(db::SimulationResultPtr);

public slots:
    bool start_generate_for(db::Entity,
                            db::Mesh                     mesh,
                            analysis::FluxMapBakeOptions options);

signals:
    void image_ready(db::Entity, analysis::BakedFluxMapPtr);
    void image_progress(db::Entity, int);

    void cancel_all();
    void cancel_specific(db::Entity);
};

} // namespace analysis