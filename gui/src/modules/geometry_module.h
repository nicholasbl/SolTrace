#pragma once

#include <QObject>
#include "utilities/qt_helpers.h"
#include "backend.h"
#include "module_common.h"

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
class GeometryModule : public QObject {
    Q_OBJECT

public:
    explicit GeometryModule(QObject* parent = nullptr);

    QOBJECT_READONLY_PROPERTY(StatusComponent, status);

           /// Non-owning reference to the geometry backend slice.
           /// Constrains QML access to geometry-specific backend functionality only.
    QPOINTER_WRITABLE_PROPERTY(GeometryBackend, backend);
};


} // namespace SolTrace::GUI::App
