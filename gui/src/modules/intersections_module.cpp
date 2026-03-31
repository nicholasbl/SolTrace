#include "intersections_module.h"

namespace SolTrace::GUI::App {

IntersectionsModule::IntersectionsModule(QObject* parent)
    : QObject(parent), m_ray_geometry(new analysis::RayGeometry) { }

void IntersectionsModule::set_results(std::shared_ptr<ResultDB> ptr) {
    m_ray_geometry->set_results(ptr);
}

analysis::RayGeometry* IntersectionsModule::ray_geometry() const {
    return m_ray_geometry.get();
}

} // namespace SolTrace::GUI::App
