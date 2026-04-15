#pragma once

#include "backend.h"
#include "database/fluxmapworldmodel.h"
#include "database/mesh_qml_bridge.h"
#include "database/rootelementsmodel.h"
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

    db::SimulationResultPtr m_results;

    QOBJECT_READONLY_PROPERTY(db::RootElementsModel, entity_model);
    QOBJECT_READONLY_PROPERTY(db::PendingFluxMapModel, pending_flux_maps);
    QOBJECT_READONLY_PROPERTY(db::FluxMapWorldModel, flux_map_world_model);

    QOBJECT_READONLY_PROPERTY(db::QMLMesh, ray_iso_volume);

    Q_WRITABLE_PROPERTY(db::Entity, current_entity, {});

    // Hack
    Q_WRITABLE_PROPERTY(QString, current_image, {});

private slots:
    void iso_surf_ready(QUuid const&, db::Mesh);
    void iso_surf_failed(QUuid const&, QString);

public:
    explicit FluxModule(QQmlEngine*, QObject* parent = nullptr);

public slots:
    void set_results(db::SimulationResultPtr);

    void start_generate();
};

} // namespace SolTrace::GUI::App
