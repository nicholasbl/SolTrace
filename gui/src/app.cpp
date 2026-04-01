#include "app.h"
#include <QSettings>

namespace SolTrace::GUI::App {

void App::load_session() {
    QSettings s;
    s.beginGroup("View");
    m_view->set_show_left_panel(s.value("show_left_panel", true).toBool());
    m_view->set_show_right_panel(s.value("show_right_panel", true).toBool());

    m_view->set_workflow_phase(s.value("workflow_phase", 0).toUInt());

    m_view->set_configure_section(s.value("configure_section", 0).toUInt());
    m_view->set_simulate_section(s.value("simulate_section", 0).toUInt());
    m_view->set_analyze_section(s.value("analyze_section", 0).toUInt());

    m_view->set_sun_section(s.value("sun_section", 0).toUInt());

    s.endGroup();

    s.beginGroup("File");
    m_file_source->set_source(s.value("source", "").toUrl());
    s.endGroup();
}

void App::save_session() {
    QSettings s;
    s.beginGroup("View");
    s.setValue("show_left_panel", m_view->show_left_panel());
    s.setValue("show_right_panel", m_view->show_right_panel());

    s.setValue("workflow_phase", m_view->workflow_phase());

    s.setValue("configure_section", m_view->configure_section());
    s.setValue("simulate_section", m_view->simulate_section());
    s.setValue("analyze_section", m_view->analyze_section());

    s.setValue("sun_section", m_view->sun_section());

    s.endGroup();

    s.beginGroup("File");
    s.setValue("source", m_file_source->source());
    s.endGroup();
}

App::App(QObject* parent, const QString& documentation_directory)
    : m_file_source(new FileSourceModule(this)),
      m_view(new ViewModule(this)),
      m_workflow(new WorkflowModule(this)),
      m_docs(new DocumentationModule(this)),
      m_sun(new SunModule(this)),
      m_tracing(new TracingModule(this)),
      m_materials(new MaterialsModule(this)),
      m_layout(new LayoutModule(this)),
      m_simulation(new SimulationModule(this)),
      m_intersections(new IntersectionsModule(this)),
      m_flux(new FluxModule(this)) {

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

    load_session();
}

App::~App() {
    save_session();
}

void App::install(QPointer<Backend> backend) {
    m_sun->set_backend(QPointer(backend->sun()));
    m_tracing->set_backend(QPointer(backend->tracing()));
    m_intersections->set_backend(QPointer(backend->intersections()));
    m_flux->set_backend(QPointer(backend->flux()));
}

} // namespace SolTrace::GUI::App
