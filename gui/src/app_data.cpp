#include "app_data.h"
#include <QApplication>
#include <QSettings>

namespace SolTrace::GUI::App {

void AppData::load_session() {
    QSettings s;
    s.beginGroup("View");
    m_view->left_panel()->set_visible(
        s.value("show_left_panel", true).toBool());
    m_view->left_panel()->set_saved_visible(
        s.value("show_left_panel_saved", true).toBool());
    m_view->right_panel()->set_visible(
        s.value("show_right_panel", true).toBool());
    m_view->right_panel()->set_saved_visible(
        s.value("show_right_panel_saved", true).toBool());
    m_view->settings_panel()->set_visible(
        s.value("show_settings_panel", true).toBool());

    m_view->left_panel()->set_width(s.value("left_panel_width", 550).toUInt());
    m_view->right_panel()->set_width(s.value("right_panel_width", 0).toUInt());

    m_view->set_workflow_phase(s.value("workflow_phase", 0).toUInt());

    m_view->set_configure_section(s.value("configure_section", 0).toUInt());
    m_view->set_simulate_section(s.value("simulate_section", 0).toUInt());
    m_view->set_analyze_section(s.value("analyze_section", 0).toUInt());

    m_view->set_sun_section(s.value("sun_section", 0).toUInt());

    m_view->set_documentation_section(
        s.value("documentation_section", 0).toUInt());

    s.endGroup();

    s.beginGroup("Sun");
    m_sun->shape()->set_shape(
        static_cast<SunShape::Shape>(s.value("shape", 0).toDouble()));
    m_sun->shape()->set_sigma(s.value("sigma", 1.551).toDouble());
    m_sun->shape()->set_half_width(s.value("half_width", 2.023).toDouble());
    m_sun->shape()->set_csr(s.value("buie_csr", 0.596).toDouble());

    auto* cdist = m_sun->shape()->custom_distribution();
    if (s.contains("custom_shape"))
        cdist->set_variant_data(s.value("custom_shape").toList());
    else
        m_sun->shape()->reset_current_distribution();
    s.endGroup();
}

void AppData::save_session() {
    QSettings s;
    s.beginGroup("View");
    s.setValue("show_left_panel", m_view->left_panel()->visible());
    s.setValue("show_right_panel", m_view->right_panel()->visible());
    s.setValue("show_left_panel_saved", m_view->left_panel()->saved_visible());
    s.setValue("show_right_panel_saved",
               m_view->right_panel()->saved_visible());
    s.setValue("show_settings_panel", m_view->settings_panel()->visible());

    s.setValue("left_panel_width", m_view->left_panel()->width());
    s.setValue("right_panel_width", m_view->right_panel()->width());

    s.setValue("workflow_phase", m_view->workflow_phase());

    s.setValue("configure_section", m_view->configure_section());
    s.setValue("simulate_section", m_view->simulate_section());
    s.setValue("analyze_section", m_view->analyze_section());

    s.setValue("sun_section", m_view->sun_section());

    s.setValue("documentation_section", m_view->documentation_section());

    s.endGroup();


    s.beginGroup("Sun");
    s.setValue("shape", static_cast<int>(m_sun->shape()->shape()));
    s.setValue("sigma", m_sun->shape()->sigma());
    s.setValue("half_width", m_sun->shape()->half_width());
    s.setValue("buie_csr", m_sun->shape()->csr());

    auto* cdist = m_sun->shape()->custom_distribution();
    s.setValue("custom_shape", cdist->variant_data());
    s.endGroup();
}

AppData* AppData::create(QQmlEngine* qmlEngine, QJSEngine*) {
    return new AppData(nullptr, qmlEngine, "");
}

AppData::AppData(QObject*       parent,
                 QQmlEngine*    engine,
                 const QString& documentation_directory)
    : m_file_source(new DatabaseModule(this)),
      m_view(new ViewModule(this)),
      m_docs(new DocumentationModule(this)),
      m_sun(new SunModule(this)),
      m_materials(new MaterialsModule(this)),
      m_layout(new LayoutModule(this)),
      m_simulation(new SimulationModule(this)),
      m_intersections(new IntersectionsModule(this)),
      m_flux(new FluxModule(engine, this)),
      m_script(new Script::Script(this)) {

    connect(m_file_source,
            &DatabaseModule::current_database_value_changed,
            this,
            &AppData::set_current_database);

    connect(
        m_file_source, &DatabaseModule::notify, this, &AppData::notification);

    connect(this,
            &AppData::current_database_value_changed,
            this,
            &AppData::new_database);

    connect(this,
            &AppData::new_database,
            m_simulation,
            &SimulationModule::set_current_database);

    connect(
        this, &AppData::new_database, m_sun, &SunModule::set_current_database);

    connect(this,
            &AppData::new_database,
            m_materials,
            &MaterialsModule::set_current_database);

    connect(this,
            &AppData::new_database,
            m_layout,
            &LayoutModule::set_current_database);

    connect(m_simulation,
            &SimulationModule::new_results,
            this,
            &AppData::new_results);

    connect(
        this, &AppData::new_database, m_script, &Script::Script::set_database);

    connect(qApp, &QCoreApplication::aboutToQuit, this, &AppData::save_session);

    connect(this,
            &AppData::new_results,
            m_intersections,
            &IntersectionsModule::set_results);

    connect(this, &AppData::new_results, m_flux, &FluxModule::set_results);

    load_session();

    m_file_source->load_new();
}

AppData::~AppData() {
    save_session();
}

} // namespace SolTrace::GUI::App
