#pragma once
#include <QAbstractListModel>
#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>

#include "utilities/qt_helpers.h"

#include <backend.h>

#include <modules/file_source_module.h>
#include <modules/module_common.h>
#include <modules/sun_module.h>
#include <modules/simulation_module.h>
#include <modules/documentation_module.h>

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

// ─── Application Modules
// ───────────────────────────────────────────────────────────────────



/**
 * @class TracingModule
 * @brief Ray tracing engine configuration module.
 *
 * Configures the ray tracing engine used for simulation execution.
 * The active tracer selection determines which backend implementation is used:
 *
 * - Legacy:     Original SolTrace tracer
 * - Refactored: Modernized CPU tracer
 * - GPU:        GPU-accelerated tracer (default)
 *
 * QML access pattern: App.tracing.max_intersections
 */
class TracingModule : public QObject {
    Q_OBJECT

public:
    TracingModule(QObject* parent = nullptr);

    QPOINTER_WRITABLE_PROPERTY(TracingBackend, backend)
    QOBJECT_READONLY_PROPERTY(StatusComponent, status)

    enum class Tracer { Legacy, Refactored, GPU };

    Q_ENUM(Tracer)

    Q_WRITABLE_PROPERTY(Tracer, tracer, Tracer::GPU)

    // ─── Engine Properties
    // ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(double, max_intersections, 3.5e6)
    Q_WRITABLE_PROPERTY(double, max_rays, 1e6)
    Q_WRITABLE_PROPERTY(bool, sun_shape, false)

    // ─── Execution Properties
    // ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(int, cpu_cores, 4)
    Q_WRITABLE_PROPERTY(int, seed_value, 12345)

    // ─── Optimization Properties
    // ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(bool, optical_errors, false)
    Q_WRITABLE_PROPERTY(bool, point_focus_system, false)
};

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
    MaterialsModule(QObject* parent = nullptr);

    QOBJECT_READONLY_PROPERTY(StatusComponent, status);

    /// Non-owning reference to the materials backend slice.
    /// Constrains QML access to materials-specific backend functionality only.
    QPOINTER_WRITABLE_PROPERTY(MaterialsBackend, backend)
};

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
    GeometryModule(QObject* parent = nullptr);

    QOBJECT_READONLY_PROPERTY(StatusComponent, status);

    /// Non-owning reference to the geometry backend slice.
    /// Constrains QML access to geometry-specific backend functionality only.
    QPOINTER_WRITABLE_PROPERTY(GeometryBackend, backend);
};

/**
 * @class WorkflowModule
 * @brief Models the application workflow structure for QML navigation.
 *
 * Encodes the three-level hierarchy: Phase → Section → Module
 *
 * - Phase:   Configuration, Simulation, Analysis
 * - Section: Sub-groupings within each phase
 *
 * The phase and section properties drive which panel is displayed in the UI.
 * The static get_*_sections() methods provide display strings for tab
 * rendering.
 *
 * Note: Phase and section enums are currently hardcoded.
 */
class WorkflowModule : public QObject {
    Q_OBJECT

public:
    WorkflowModule(QObject* parent = nullptr);

    enum class Phase { Configuration, Simulation, Analysis };
    enum class ConfigurationSection { Sun, Tracing, Materials, Geometry };
    enum class SimulationSection { Execution, Navigation, Diagnostics };
    enum class AnalysisSection { Intersections, Flux };

    Q_ENUM(Phase)
    Q_ENUM(ConfigurationSection)
    Q_ENUM(SimulationSection)
    Q_ENUM(AnalysisSection)

    Q_WRITABLE_PROPERTY(Phase, phase, Phase::Configuration)
    Q_WRITABLE_PROPERTY(ConfigurationSection,
                        configuration_section,
                        ConfigurationSection::Sun);
    Q_WRITABLE_PROPERTY(SimulationSection,
                        simulation_section,
                        SimulationSection::Execution);
    Q_WRITABLE_PROPERTY(AnalysisSection,
                        analysis_section,
                        AnalysisSection::Intersections);

    Q_INVOKABLE static QStringList phases();
    Q_INVOKABLE static QStringList configuration_sections();
    Q_INVOKABLE static QStringList simulation_sections();
    Q_INVOKABLE static QStringList analysis_sections();
};

/**
 * @class IntersectionsModule
 * @brief Ray intersection analysis module.
 *
 * Provides access to intersection results from the simulation.
 * Holds non-owning references to both the shared results backend
 * and the intersections-specific backend.
 *
 * QML access pattern: App.intersections.results
 */
class IntersectionsModule : public QObject {
    Q_OBJECT

public:
    IntersectionsModule(QObject* parent = nullptr);

    QPOINTER_WRITABLE_PROPERTY(ResultsBackend, results)
    QPOINTER_WRITABLE_PROPERTY(IntersectionsBackend, backend)
};

/**
 * @class FluxModule
 * @brief Flux analysis module.
 *
 * Provides access to flux distribution results from the simulation.
 * Shares ResultsBackend with Intersections — both modules read from
 * the same simulation result data.
 *
 * QML access pattern: App.flux.results
 */
class FluxModule : public QObject {
    Q_OBJECT

public:
    FluxModule(QObject* parent = nullptr);

    QPOINTER_WRITABLE_PROPERTY(ResultsBackend, results)
    QPOINTER_WRITABLE_PROPERTY(FluxBackend, backend)
};

// ─── QML Entrypoint ───────────────────────────────────────────────────────────────────

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
    App(QObject* parent = nullptr, const QString& documentation_directory = "");

    QOBJECT_WRITABLE_PROPERTY(db::Database, current_database)

    /// App.file_source.source
    QOBJECT_READONLY_PROPERTY(FileSourceModule, file_source)

    /// App.WorkflowModule.phases()
    QOBJECT_READONLY_PROPERTY(WorkflowModule, workflow)

    /// App.docs.get("configuration.sun.directional_sun")
    QOBJECT_READONLY_PROPERTY(DocumentationModule, docs)

    /// App.sun.definition.sun_type
    QOBJECT_READONLY_PROPERTY(SunModule, sun)

    /// App.tracing.max_intersections
    QOBJECT_READONLY_PROPERTY(TracingModule, tracing)

    /// App.materials.backend.child_model
    QOBJECT_READONLY_PROPERTY(MaterialsModule, materials)

    /// App.geometry.backend.world_geometry_model
    QOBJECT_READONLY_PROPERTY(GeometryModule, geometry)

    /// App.simulation.start()
    QOBJECT_READONLY_PROPERTY(SimulationModule, simulation)

    /// App.intersections.results
    QOBJECT_READONLY_PROPERTY(IntersectionsModule, intersections)

    /// App.flux.results
    QOBJECT_READONLY_PROPERTY(FluxModule, flux)

    void install(QPointer<Backend> backend);

signals:
    void notification(ANotification);
    void new_results(std::shared_ptr<ResultDB>);
    void new_database(db::Database*);
};

} // namespace SolTrace::GUI::App
