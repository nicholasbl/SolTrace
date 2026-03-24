#include "layout_module.h"


namespace SolTrace::GUI::App {

void LayoutModule::new_entity_selected() {
    child_model()->set_node(m_current_element);
    breadcrumb_model()->set_node(m_current_element);
    instance_edit()->set(m_current_element);
}

LayoutModule::LayoutModule(QObject* parent)
    : QObject(parent),
      m_status(new StatusComponent(this)),
      m_root_elements_model(new db::RootElementsModel(this)),
      m_child_model(new db::ChildModel(this)),
      m_breadcrumb_model(new db::BreadcrumbModel(this)),
      m_instance_edit(new db::AnInstanceEditor(this)),
      m_world_geometry_model(new db::WorldGeometryModel(this)) {

    connect(this,
            &LayoutModule::current_database_value_changed,
            m_root_elements_model,
            &db::RootElementsModel::reset);

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

    connect(this,
            &LayoutModule::current_element_changed,
            this,
            &LayoutModule::new_entity_selected);
}

} // namespace SolTrace::GUI::App
