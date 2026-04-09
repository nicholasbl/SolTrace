#pragma once

#include <QImage>
#include <QObject>

#include "database/mesh.h"
#include "utilities/qt_helpers.h"

#include "job_control/job_run_common.h"

namespace analysis {

struct FluxMapBakeOptions {
    glm::uvec2 bin_counts       = { 128, 128 };
    glm::uvec2 image_resolution = { 1024, 1024 };
    QColor     grid_line_color  = QColor();
    QImage     color_map;
};

class FluxMapComputer : public QObject {
    Q_OBJECT

    std::shared_ptr<db::SimulationResult const> m_database;

public:
    explicit FluxMapComputer(QObject* parent);
    ~FluxMapComputer() override;

    void set_results(std::shared_ptr<db::SimulationResult const>);

public slots:
    bool
    start_generate_for(db::Entity, db::Mesh mesh, FluxMapBakeOptions options);

signals:
    void image_ready(db::Entity, QImage);
    void image_progress(db::Entity, int);

    void cancel_all();
    void cancel_specific(db::Entity);
};

} // namespace analysis