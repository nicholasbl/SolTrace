#pragma once

#include <QObject>
#include "utilities/qt_helpers.h"
#include "backend.h"
#include "module_common.h"

namespace SolTrace::GUI::App {

/**
 * @class IntersectionsModule
 * @brief Ray intersection analysis module.
 *
 * Provides access to intersection results from the simulation.
 * Holds non-owning references to both the shared results backend
 * and the intersections-specific backend.
 *
 * QML access pattern: App.intersections.results
 */
class IntersectionsModule : public QObject {
    Q_OBJECT

    std::shared_ptr<db::SimulationResult> m_results;

    std::unique_ptr<analysis::RayGeometry> m_ray_geometry;

    Q_PROPERTY(analysis::RayGeometry* ray_geometry READ ray_geometry NOTIFY
                   ray_geometry_changed FINAL)

public:
    explicit IntersectionsModule(QObject* parent = nullptr);


    analysis::RayGeometry* ray_geometry() const;

public slots:
    void set_results(std::shared_ptr<db::SimulationResult>);

signals:
    void ray_geometry_changed();
};


} // namespace SolTrace::GUI::App
