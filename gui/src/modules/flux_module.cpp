#include "flux_module.h"

#include <QQmlEngine>

namespace SolTrace::GUI::App {

FluxModule::FluxModule(QQmlEngine* engine, QObject* parent)
    : QObject(parent),
      m_entity_model(new db::RootElementsModel(this)),
      m_pending_flux_maps(new db::PendingFluxMapModel(this)),
      m_flux_map_world_model(new db::FluxMapWorldModel(this)) {
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
    m_entity_model->reset(p->database.get());
    m_pending_flux_maps->reset(p);

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
    m_pending_flux_maps->start_generate_for(current_entity());
}

} // namespace SolTrace::GUI::App
