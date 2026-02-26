#pragma once

#include <QObject>
#include "utilities/qt_helpers.h"
#include "backend.h"
#include "module_common.h"

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

public:
    explicit MaterialsModule(QObject* parent = nullptr);

    QOBJECT_READONLY_PROPERTY(StatusComponent, status);

    /// Non-owning reference to the materials backend slice.
    /// Constrains QML access to materials-specific backend functionality only.
    QPOINTER_WRITABLE_PROPERTY(MaterialsBackend, backend)
};

} // namespace SolTrace::GUI::App

