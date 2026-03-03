#include "materials_module.h"


namespace SolTrace::GUI::App {

MaterialsModule::MaterialsModule(QObject* parent)
    : QObject(parent),
      m_status(new StatusComponent(this)),
      m_materials_list(new db::RenderGroupsModel(this)),
      m_group_edit(new db::GroupEditor(this)) {

    connect(this,
            &MaterialsModule::current_database_value_changed,
            m_materials_list,
            &db::RenderGroupsModel::reset);

    connect(this,
            &MaterialsModule::current_database_changed,
            this,
            &MaterialsModule::select_first_material);

    connect(this,
            &MaterialsModule::current_material_changed,
            this,
            &MaterialsModule::new_material_selected);
}

void MaterialsModule::select_first_material() {
    // TODO: this seems to be fired a lot
    qDebug() << Q_FUNC_INFO;
    for (auto const& [e, comp] : current_database()->group_root.view().each()) {
        qDebug() << Q_FUNC_INFO << "Selecting " << entt::to_integral(e);
        set_current_material(e);
    }

    set_current_material(db::Entity());
}

void MaterialsModule::new_material_selected() {
    qDebug() << Q_FUNC_INFO;
    group_edit()->set(m_current_database, m_current_material);

    set_current_material_name(m_current_database->name_of(m_current_material));
}

} // namespace SolTrace::GUI::App

