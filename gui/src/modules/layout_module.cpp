#include "layout_module.h"


namespace SolTrace::GUI::App {

void LayoutModule::new_entity_selected() {
    child_model()->set_node(m_current_element);
    breadcrumb_model()->set_node(m_current_element);
    instance_edit()->set(m_current_element);

    if (!m_current_database) {
        set_current_element_name(QString());
        return;
    }

    set_current_element_name(m_current_database->name_of(m_current_element));
}

void LayoutModule::reset(db::Database* database) {
    if (m_observed_database) {
        disconnect(
            m_observed_database->identity.self(), nullptr, this, nullptr);
    }

    m_observed_database = database;

    if (!database) {
        set_current_element_name(QString());
        return;
    }

    connect(database->identity.self(),
            &db::ComponentAPIBase::changed,
            this,
            &LayoutModule::identity_changed);

    set_current_element_name(database->name_of(m_current_element));
}

void LayoutModule::identity_changed(entt::entity entity) {
    if (!m_current_database) return;

    if (db::Entity(entity) != m_current_element) return;

    set_current_element_name(m_current_database->name_of(m_current_element));
}

LayoutModule::LayoutModule(QObject* parent)
    : QObject(parent),
      m_status(new StatusComponent(this)),
      m_all_elements_model(new db::AllElementsModel(this)),
      m_root_elements_model(new db::RootElementsModel(this)),
      m_filtered_root_elements_model(new db::InstanceSortFilter(this)),
      m_child_model(new db::ChildModel(this)),
      m_breadcrumb_model(new db::BreadcrumbModel(this)),
      m_instance_edit(new db::AnInstanceEditor(this)),
      m_world_geometry_model(new db::WorldGeometryModel(this)) {

    m_filtered_root_elements_model->setSourceModel(m_root_elements_model);

    connect(this,
            &LayoutModule::current_database_value_changed,
            m_all_elements_model,
            &db::AllElementsModel::reset);

    connect(this,
            &LayoutModule::current_database_value_changed,
            m_root_elements_model,
            &db::RootElementsModel::reset);

    connect(this,
            &LayoutModule::current_database_value_changed,
            this,
            &LayoutModule::reset);

    connect(this,
            &LayoutModule::current_database_value_changed,
            m_filtered_root_elements_model,
            &db::InstanceSortFilter::reset);

    connect(this,
            &LayoutModule::current_database_value_changed,
            m_child_model,
            &db::ChildModel::reset);

    connect(this,
            &LayoutModule::current_database_value_changed,
            m_breadcrumb_model,
            &db::BreadcrumbModel::reset);

    connect(this,
            &LayoutModule::current_database_value_changed,
            m_instance_edit,
            &db::AnInstanceEditor::reset);

    connect(this,
            &LayoutModule::current_database_value_changed,
            m_world_geometry_model,
            &db::WorldGeometryModel::reset);

    // Element changes

    connect(this,
            &LayoutModule::current_element_changed,
            this,
            &LayoutModule::new_entity_selected);
}

} // namespace SolTrace::GUI::App
