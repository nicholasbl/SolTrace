#include "app.h"


namespace SolTrace::GUI::App {


App::App(QObject* parent, const QString& documentation_directory)
    : m_file_source(new FileSourceModule(this)),
      m_workflow(new WorkflowModule(this)),
      m_docs(new DocumentationModule(this)),
      m_sun(new SunModule(this)),
      m_tracing(new TracingModule(this)),
      m_materials(new MaterialsModule(this)),
      m_layout(new LayoutModule(this)),
      m_simulation(new SimulationModule(this)),
      m_intersections(new IntersectionsModule(this)),
      m_flux(new FluxModule(this)) {

    m_docs->load();

    connect(m_file_source,
            &FileSourceModule::current_database_value_changed,
            this,
            [this](auto* ptr) { emit this->new_database(ptr); });

    connect(this,
            &App::new_database,
            m_simulation,
            &SimulationModule::set_current_database);

    connect(this,
            &App::new_database,
            m_materials,
            &MaterialsModule::set_current_database);

    connect(this,
            &App::new_database,
            m_layout,
            &LayoutModule::set_current_database);

    connect(
        m_simulation, &SimulationModule::new_results, this, &App::new_results);
}

void App::install(QPointer<Backend> backend)
{
    m_sun->set_backend(QPointer(backend->sun()));
    m_tracing->set_backend(QPointer(backend->tracing()));
    m_intersections->set_backend(QPointer(backend->intersections()));
    m_flux->set_backend(QPointer(backend->flux()));
}


} // namespace SolTrace::GUI::App
