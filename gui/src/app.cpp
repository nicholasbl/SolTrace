#include "app.h"


namespace SolTrace::GUI::App {

// Simlation ===================================================================


// App =========================================================================

App::App(QObject* parent, const QString& documentation_directory)
    : m_workflow(new Workflow(this)),
      m_docs(new Documentation(this)),
      m_sun(new Sun(this)),
      m_tracing(new Tracing(this)),
      m_materials(new Materials(this)),
      m_geometry(new Geometry(this)),
      m_simulation(new SimulationModule(this)),
      m_intersections(new Intersections(this)),
      m_flux(new Flux(this)) {

    m_docs->load();

    connect(this,
            &App::new_database,
            m_simulation,
            &SimulationModule::set_current_database);

    connect(
        m_simulation, &SimulationModule::new_results, this, &App::new_results);
}


} // namespace SolTrace::GUI::App
