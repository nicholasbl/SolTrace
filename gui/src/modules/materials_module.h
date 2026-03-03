#pragma once

#include "database/database.h"
#include "database/database_models.h"
#include "database/geometryeditor.h"
#include "module_common.h"
#include "utilities/qt_helpers.h"

#include <QObject>

namespace SolTrace::GUI::App {

/**
 * @class MaterialsModule
 * @brief Materials configuration module.
 *
 * Provides QML access to the materials database models owned by
 * MaterialsBackend. This module does not own the models — it holds a non-owning
 * QPointer reference to the backend slice, constraining access to
 * materials-specific functionality.
 *
 * QML access pattern: App.materials.backend.child_model
 */
class MaterialsModule : public QObject {
    Q_OBJECT


private slots:
    void select_first_material();
    void new_material_selected();

public:
    explicit MaterialsModule(QObject* parent = nullptr);

    QOBJECT_READONLY_PROPERTY(StatusComponent, status);
    QOBJECT_WRITABLE_PROPERTY(db::Database, current_database)
    QOBJECT_WRITABLE_PROPERTY(db::RenderGroupsModel, materials_list)
    QOBJECT_WRITABLE_PROPERTY(db::GroupEditor, group_edit);

    Q_WRITABLE_PROPERTY(db::Entity, current_material, {})
    Q_READONLY_PROPERTY(QString, current_material_name)
};

} // namespace SolTrace::GUI::App

