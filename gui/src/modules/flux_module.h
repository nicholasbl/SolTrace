#pragma once

#include "backend.h"
#include "database/fluxmapworldmodel.h"
#include "module_common.h"
#include "utilities/qt_helpers.h"
#include <QObject>

namespace SolTrace::GUI::App {

/**
 * @class FluxModule
 * @brief Flux analysis module.
 *
 * Provides access to flux distribution results from the simulation.
 * Shares ResultsBackend with Intersections — both modules read from
 * the same simulation result data.
 *
 * QML access pattern: App.flux.results
 */
class FluxModule : public QObject {
    Q_OBJECT

    std::shared_ptr<db::SimulationResult> m_results;

    QOBJECT_READONLY_PROPERTY(db::PendingFluxMapModel, pending_flux_maps);
    QOBJECT_READONLY_PROPERTY(db::FluxMapWorldModel, flux_map_world_model);

public:
    explicit FluxModule(QQmlEngine*, QObject* parent = nullptr);

public slots:
    void set_results(db::SimulationResult*);
};

} // namespace SolTrace::GUI::App
