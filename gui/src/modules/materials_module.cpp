#include "materials_module.h"

namespace SolTrace::GUI::App {

MaterialsModule::MaterialsModule(QObject* parent)
    : QObject(parent),
      m_status(new StatusComponent(this)),
      m_materials_list(new db::MaterialGroupsModel(this)),
      m_geometry_list(new db::GeometryGroupsModel(this)),
      m_material_edit(new db::MaterialEditor(this)),
      m_geometry_edit(new db::GeometryEditor(this)) {

    connect(this,
            &MaterialsModule::current_database_value_changed,
            m_materials_list,
            &db::MaterialGroupsModel::reset);

    connect(this,
            &MaterialsModule::current_database_value_changed,
            m_geometry_list,
            &db::GeometryGroupsModel::reset);

    // connect(this,
    //         &MaterialsModule::current_database_changed,
    //         this,
    //         &MaterialsModule::select_first_material);

    connect(this,
            &MaterialsModule::current_material_changed,
            this,
            &MaterialsModule::new_material_selected);

    connect(this,
            &MaterialsModule::current_geometry_changed,
            this,
            &MaterialsModule::new_geometry_selected);
}

// void MaterialsModule::select_first_material() {
//     // TODO: this seems to be fired a lot
//     qDebug() << Q_FUNC_INFO;
//     for (auto const& [e, comp] :
//          current_database()->material_parameters.view().each()) {
//         qDebug() << Q_FUNC_INFO << "Selecting " << entt::to_integral(e);
//         set_current_material(e);
//     }

//     set_current_material(db::Entity());
// }

void MaterialsModule::new_material_selected() {
    qDebug() << Q_FUNC_INFO;
    material_edit()->set(m_current_database, m_current_material);

    set_current_material_name(m_current_database->name_of(m_current_material));
}

void MaterialsModule::new_geometry_selected() {
    qDebug() << Q_FUNC_INFO;
    geometry_edit()->set(m_current_database, m_current_geometry);

    set_current_geometry_name(m_current_database->name_of(m_current_geometry));
}

} // namespace SolTrace::GUI::App
