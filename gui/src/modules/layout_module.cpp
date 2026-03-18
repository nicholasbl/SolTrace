#include "layout_module.h"


namespace SolTrace::GUI::App {

LayoutModule::LayoutModule(QObject* parent)
    : QObject(parent),
      m_status(new StatusComponent(this)),
      m_child_model(new db::ChildModel(this)),
      m_breadcrumb_model(new db::BreadcrumbModel(this)),
      m_instance_edit(new db::AnInstanceEditor(this)) {

    connect(this,
            &LayoutModule::current_database_value_changed,
            m_child_model,
            &db::ChildModel::reset);
}

} // namespace SolTrace::GUI::App

