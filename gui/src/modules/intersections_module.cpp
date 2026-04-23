#include "intersections_module.h"

namespace SolTrace::GUI::App {

IntersectionsModule::IntersectionsModule(QObject* parent)
    : QObject(parent), m_ray_geometry(new analysis::RayGeometry) {
    m_ray_geometry->setParent(this);
}

void IntersectionsModule::set_results(db::SimulationResultPtr ptr) {
    m_ray_geometry->set_results(ptr);
}

} // namespace SolTrace::GUI::App
