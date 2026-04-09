#pragma once
#include <QAbstractListModel>
#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>

#include "utilities/qt_helpers.h"

#include <backend.h>

#include <modules/documentation_module.h>
#include <modules/file_source_module.h>
#include <modules/flux_module.h>
#include <modules/intersections_module.h>
#include <modules/layout_module.h>
#include <modules/materials_module.h>
#include <modules/module_common.h>
#include <modules/simulation_module.h>
#include <modules/sun_module.h>
#include <modules/tracing_module.h>
#include <modules/view_module.h>
#include <modules/workflow_module.h>

/**
 * @namespace SolTrace::GUI::App
 * @brief Application layer between QML and the simulation backend.
 *
 * This namespace defines the GUI application layer for SolTrace. It mediates
 * between the QML presentation layer and the simulation backend, providing:
 *
 * - Domain-driven module decomposition (Sun, Tracing, Materials, Geometry,
 * etc.)
 * - Status lifecycle tracking per module
 * - Preset management for user-configurable parameters
 * - Inline documentation loaded from markdown files
 * - Non-owning references to backend services via QPointer
 *
 * Architecture:
 * @code
 *   QML  →  App (singleton)  →  Domain Modules  →  Backend (singleton)
 * @endcode
 *
 * Each domain module holds a QPointer to its corresponding backend slice,
 * constraining access and making dependencies explicit.
 */

namespace SolTrace::GUI::App {
/**
 * @class App
 * @brief QML singleton — top-level entrypoint for the application layer.
 *
 * Provides a single, stable access point for all application modules.
 * Registered as a QML singleton so all components share one instance.
 *
 * ## Initialization
 * After construction, wire backend references by calling the appropriate
 * install methods on each module before the QML engine loads.
 *
 * @code
 *   // main.cpp
 *   auto* app = App::instance();
 *   app->docs().load();
 *   app->sun().backend() = QPointer(backend->sun_backend());
 *   // etc.
 * @endcode
 *
 */
class App : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit App(QObject*       parent                  = nullptr,
                 QString const& documentation_directory = "");

    QOBJECT_WRITABLE_PROPERTY(db::Database, current_database)

    QOBJECT_READONLY_PROPERTY(FileSourceModule, file_source)

    QOBJECT_READONLY_PROPERTY(WorkflowModule, workflow)

    QOBJECT_READONLY_PROPERTY(DocumentationModule, docs)

    QOBJECT_READONLY_PROPERTY(SunModule, sun)

    QOBJECT_READONLY_PROPERTY(TracingModule, tracing)

    QOBJECT_READONLY_PROPERTY(MaterialsModule, materials)

    QOBJECT_READONLY_PROPERTY(LayoutModule, layout)

    QOBJECT_READONLY_PROPERTY(ViewModule, view)

    QOBJECT_READONLY_PROPERTY(SimulationModule, simulation)

    QOBJECT_READONLY_PROPERTY(IntersectionsModule, intersections)

    QOBJECT_READONLY_PROPERTY(FluxModule, flux)

signals:
    void notification(ANotification);
    void new_results(std::shared_ptr<db::SimulationResult>);
    void new_database(db::Database*);
};

} // namespace SolTrace::GUI::App
