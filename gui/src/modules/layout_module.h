#pragma once

#include "backend.h"
#include "database/rootelementsmodel.h"
#include "module_common.h"
#include "utilities/qt_helpers.h"
#include <QObject>

namespace SolTrace::GUI::App {


/**
 * @class GeometryModule
 * @brief Geometry configuration module.
 *
 * Provides QML access to the geometry database models owned by GeometryBackend.
 * Holds a non-owning QPointer reference to its backend slice.
 *
 * QML access pattern: App.geometry.backend.world_geometry_model
 */
class LayoutModule : public QObject {
    Q_OBJECT

private slots:
    void new_entity_selected();

public:
    explicit LayoutModule(QObject* parent = nullptr);

    QOBJECT_WRITABLE_PROPERTY(db::Database, current_database)

    QOBJECT_READONLY_PROPERTY(db::RootElementsModel, root_elements_model);
    QOBJECT_WRITABLE_PROPERTY(db::ChildModel, child_model);
    QOBJECT_WRITABLE_PROPERTY(db::BreadcrumbModel, breadcrumb_model);
    QOBJECT_WRITABLE_PROPERTY(db::AnInstanceEditor, instance_edit);
    QOBJECT_READONLY_PROPERTY(db::WorldGeometryModel, world_geometry_model);

    Q_WRITABLE_PROPERTY(db::Entity, current_element, { })

    /// we need a selected element lists. need global pos and rot

    QOBJECT_READONLY_PROPERTY(StatusComponent, status);
};


} // namespace SolTrace::GUI::App
