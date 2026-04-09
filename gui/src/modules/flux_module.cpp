#include "flux_module.h"

#include <QQmlEngine>

namespace SolTrace::GUI::App {

FluxModule::FluxModule(QQmlEngine* engine, QObject* parent)
    : QObject(parent),
      m_pending_flux_maps(new db::PendingFluxMapModel(this)),
      m_flux_map_world_model(new db::FluxMapWorldModel(this)) {
    auto provider = m_pending_flux_maps->make_new_provider();

    engine->addImageProvider("fluxmap", provider);
}

void FluxModule::set_results(db::SimulationResult* p) {
    m_pending_flux_maps->reset(p);
}

} // namespace SolTrace::GUI::App

