#include "flux_module.h"
#include "analysis/volume_to_mesh.h"
#include "utilities/asynctask.h"

#include <QQmlEngine>
#include <QUuid>

namespace SolTrace::GUI::App {

FluxModule::FluxModule(QQmlEngine* engine, QObject* parent)
    : QObject(parent),
      m_entity_model(new db::RootElementsModel(this)),
      m_pending_flux_maps(new db::PendingFluxMapModel(this)),
      m_flux_map_world_model(new db::FluxMapWorldModel(this)),
      m_ray_iso_volume(new db::QMLMesh()) {

    m_ray_iso_volume->setParent(this);

    auto provider = m_pending_flux_maps->make_new_provider();

    engine->addImageProvider("fluxmap", provider);

    connect(m_pending_flux_maps,
            &db::PendingFluxMapModel::ready,
            m_flux_map_world_model,
            &db::FluxMapWorldModel::on_ready);

    connect(m_pending_flux_maps,
            &db::PendingFluxMapModel::cleared,
            m_flux_map_world_model,
            &db::FluxMapWorldModel::on_reset);
}

void FluxModule::set_results(db::SimulationResultPtr p) {
    m_results = p;
    m_entity_model->reset(p->database.get());
    m_pending_flux_maps->reset(p);
    m_ray_iso_volume->set_current_mesh({});

    // HACK HACK HACK

    entt::entity largest = entt::null;
    size_t       best    = 0;

    for (auto& [c, v] : p->entity_to_ray_ids) {
        if (v.size() > best) {
            largest = c;
            best    = v.size();
        }
    }

    set_current_entity(largest);
}

void FluxModule::start_generate() {
    qDebug() << Q_FUNC_INFO << "Starting fluxmap generation for current entity";
    m_pending_flux_maps->start_generate_for(current_entity());

    // HACK HACK HACK

    qDebug() << Q_FUNC_INFO << "results: " << !!m_results;

    if (m_results) {
        qDebug() << Q_FUNC_INFO << "launching volume generation";
        launch_async_task<db::Mesh>(QUuid::createUuid(),
                                    this,
                                    &FluxModule::iso_surf_ready,
                                    &FluxModule::iso_surf_failed,
                                    analysis::volume_to_mesh,
                                    m_results->ray_volume,
                                    glm::vec3(m_results->bounds_min),
                                    glm::vec3(m_results->bounds_max),
                                    0.9);
    }
}

void FluxModule::iso_surf_ready(QUuid const& id, db::Mesh mesh) {
    qDebug() << Q_FUNC_INFO << id;
    m_ray_iso_volume->set_current_mesh(mesh);
}
void FluxModule::iso_surf_failed(QUuid const& id, QString reason) {
    qDebug() << Q_FUNC_INFO << id;
    qCritical() << "Unable to generate isosurface" << reason;
}

} // namespace SolTrace::GUI::App
