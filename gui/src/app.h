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
#include <modules/simulationmodule.h>

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

// ─── Application Components
// ───────────────────────────────────────────────────────────────────


/**
 * @class PresetComponentBase
 * @brief QML-facing base class for preset management.
 *
 * Exposes preset operations to QML via Q_INVOKABLE methods.
 * Inherits QAbstractListModel so QML can display the list of available
 * presets directly (name + description per row).
 *
 * Actual file I/O is delegated to the backend's file utilities service.
 * This class is the thin QML adapter — it does not own persistence logic.
 *
 * Roles:
 * - NameRole:        Display name of the preset
 * - DescriptionRole: Optional description string
 */
class PresetComponentBase : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { NameRole = Qt::UserRole + 1, DescriptionRole };

    Q_INVOKABLE bool load(const QString& name);
    Q_INVOKABLE bool save(const QString& name, const QString& description = "");
    Q_INVOKABLE bool remove(const QString& name);
    Q_INVOKABLE bool import_preset(const QString& filepath);
    Q_INVOKABLE bool export_preset(const QString& name,
                                   const QString& filepath);

    int      rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index,
                  int                role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};

/**
 * @class PresetComponent
 * @brief Typed preset storage and active selection management.
 *
 * Template subclass of PresetComponentBase that adds typed storage
 * and tracks the currently active preset.
 *
 * @tparam T The domain type being managed (e.g. SunDefinition,
 * DirectionalSunPosition)
 *
 * active_preset reflects the currently loaded configuration. QML binds
 * to this to display and edit the active parameters. Changing active_preset
 * does not automatically save — the user must explicitly call save().
 *
 * m_keys preserves insertion order for stable row indexing in the list model.
 */

template <typename T>
class PresetComponent : public PresetComponentBase {
public:
    QPointer<T> active() const;
    void        set_active(T* preset);

private:
    QHash<QString, T*> m_presets;
    QList<QString>     m_keys;
    QPointer<T>        m_active_preset;
};

// ─── Application Modules
// ───────────────────────────────────────────────────────────────────


/**
 * @class SunDefinition
 * @brief Sun type and emission profile parameters.
 *
 * Defines what kind of sun is being simulated and how its light is emitted.
 * Separated from position to allow reuse across multiple position
 * configurations — the same emission profile can be evaluated at different
 * times and locations without duplication.
 *
 * Active shape parameters (std, half_width, csr, num_points) are all stored
 * simultaneously; only the fields relevant to the active sun_shape are used
 * by the backend bridge.
 *
 * Bridges to: SolTrace::Data::Sun via SunBackend::apply_definition()
 */
class SunDefinition : public QObject {
    Q_OBJECT
public:
    SunDefinition(QObject* parent = nullptr);

    enum class SunType { Directional, PointSource };
    enum class SunShape { Gaussian, Pillbox, CSR, Custom };

    Q_ENUM(SunType)
    Q_ENUM(SunShape)

    Q_WRITABLE_PROPERTY(SunType, sun_type, SunType::Directional)
    Q_WRITABLE_PROPERTY(SunShape, sun_shape, SunShape::Gaussian)

    // Gaussian
    Q_WRITABLE_PROPERTY(double, std, 5.18)

    // Pillbox
    Q_WRITABLE_PROPERTY(double, half_width, 4.65)

    // Buie
    Q_WRITABLE_PROPERTY(double, csr, 2.0)

    // Custom Sun Shape
    Q_WRITABLE_PROPERTY(int, num_points, 1)
};

/**
 * @class DirectionalSunPosition
 * @brief Solar position parameters for a directional (infinite-distance) sun.
 *
 * Encapsulates all inputs required to calculate the sun's angular position
 * in the sky. The active position_calculator determines which subset of
 * parameters is used:
 *
 * - Legacy:  Uses azimuth/elevation directly
 * - Duffie:  Uses latitude, longitude, date, time
 * - SOLPOS:  Uses latitude, longitude, date, time, interval
 * - SPA:     Uses all fields including optional pressure/temperature
 * corrections
 *
 * Multiple DirectionalSunPosition presets can be saved and switched between,
 * enabling comparison of the same system across different times or locations.
 *
 * Bridges to: SolTrace::Data::Sun via SunBackend::apply_directional_position()
 */
class DirectionalSunPosition : public QObject {
    Q_OBJECT

public:
    DirectionalSunPosition(QObject* parent = nullptr);

    QOBJECT_READONLY_PROPERTY(PresetComponent<DirectionalSunPosition>,
                              preset_manager)

    enum class PositionCalculator { Legacy, Duffie, SOLPOS, SPA };

    Q_ENUM(PositionCalculator)

    Q_WRITABLE_PROPERTY(PositionCalculator,
                        position_calculator,
                        PositionCalculator::Legacy)

    // Position
    Q_WRITABLE_PROPERTY(double, latitude, 35.04)
    Q_WRITABLE_PROPERTY(double, longitude, -105.10)

    // Date
    Q_WRITABLE_PROPERTY(int, year, 2026)
    Q_WRITABLE_PROPERTY(int, month, 12)
    Q_WRITABLE_PROPERTY(int, day, 25)

    // Time
    Q_WRITABLE_PROPERTY(int, hour, 14)
    Q_WRITABLE_PROPERTY(int, minute, 30)
    Q_WRITABLE_PROPERTY(int, second, 0)
    Q_WRITABLE_PROPERTY(int, timezone, -7)

    // SOLPOS
    Q_WRITABLE_PROPERTY(int, interval, 1) ///< Averaging interval in seconds

    // SPA Optional fields
    Q_WRITABLE_PROPERTY(bool, optional_spa_fields, false)
    Q_WRITABLE_PROPERTY(double, dut1, 0.0)
    Q_WRITABLE_PROPERTY(double, altitude, 1000)
    Q_WRITABLE_PROPERTY(double, pressure, 1013.25)
    Q_WRITABLE_PROPERTY(double, temperature, 20.0)
};

/**
 * @class PointSourceSunPosition
 * @brief Solar position parameters for a point-source (finite-distance) sun.
 *
 * Represents a sun at a finite xyz position that emits divergent rays,
 * as opposed to the parallel rays of a directional sun.
 *
 * Bridges to: SolTrace::Data::Sun via SunBackend::apply_point_source_position()
 */
class PointSourceSunPosition : public QObject {
    Q_OBJECT

public:
    PointSourceSunPosition(QObject* parent = nullptr);

    QOBJECT_READONLY_PROPERTY(PresetComponent<PointSourceSunPosition>,
                              preset_manager)

    Q_WRITABLE_PROPERTY(double, x, 1000)
    Q_WRITABLE_PROPERTY(double, y, 1000)
    Q_WRITABLE_PROPERTY(double, z, 1000)
};

/**
 * @class Sun
 * @brief Top-level sun configuration module.
 *
 * Aggregates all sun-related components into a single QML-accessible object.
 *
 * - definition:   Sun type and emission profile (one active preset)
 * - ds_positions: Named directional sun positions (multiple presets)
 * - pss_positions: Named point source positions (multiple presets)
 * - status:       Module readiness state
 * - backend:      Non-owning reference to SunBackend for simulation bridging
 *
 * QML access pattern: App.sun.definition.sun_type
 */
class Sun : public QObject {
    Q_OBJECT

public:
    Sun(QObject* parent = nullptr) : QObject(parent) { }

    QPOINTER_WRITABLE_PROPERTY(SunBackend, backend)
    QOBJECT_READONLY_PROPERTY(StatusComponent, status)
    QOBJECT_READONLY_PROPERTY(PresetComponent<SunDefinition>, definition)
    QOBJECT_READONLY_PROPERTY(PresetComponent<DirectionalSunPosition>,
                              ds_positions)
    QOBJECT_READONLY_PROPERTY(PresetComponent<PointSourceSunPosition>,
                              pss_positions)
};

/**
 * @class Tracing
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
class Tracing : public QObject {
    Q_OBJECT

public:
    Tracing(QObject* parent = nullptr);

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
 * @class Materials
 * @brief Materials configuration module.
 *
 * Provides QML access to the materials database models owned by
 * MaterialsBackend. This module does not own the models — it holds a non-owning
 * QPointer reference to the backend slice, constraining access to
 * materials-specific functionality.
 *
 * QML access pattern: App.materials.backend.child_model
 */
class Materials : public QObject {
    Q_OBJECT

public:
    Materials(QObject* parent = nullptr);

    QOBJECT_READONLY_PROPERTY(StatusComponent, status);

    /// Non-owning reference to the materials backend slice.
    /// Constrains QML access to materials-specific backend functionality only.
    QPOINTER_WRITABLE_PROPERTY(MaterialsBackend, backend)
};

/**
 * @class Geometry
 * @brief Geometry configuration module.
 *
 * Provides QML access to the geometry database models owned by GeometryBackend.
 * Holds a non-owning QPointer reference to its backend slice.
 *
 * QML access pattern: App.geometry.backend.world_geometry_model
 */
class Geometry : public QObject {
    Q_OBJECT

public:
    Geometry(QObject* parent = nullptr);

    QOBJECT_READONLY_PROPERTY(StatusComponent, status);

    /// Non-owning reference to the geometry backend slice.
    /// Constrains QML access to geometry-specific backend functionality only.
    QPOINTER_WRITABLE_PROPERTY(GeometryBackend, backend);
};

/**
 * @class Workflow
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
class Workflow : public QObject {
    Q_OBJECT

public:
    Workflow(QObject* parent = nullptr);

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
 * @class Intersections
 * @brief Ray intersection analysis module.
 *
 * Provides access to intersection results from the simulation.
 * Holds non-owning references to both the shared results backend
 * and the intersections-specific backend.
 *
 * QML access pattern: App.intersections.results
 */
class Intersections : public QObject {
    Q_OBJECT

public:
    Intersections(QObject* parent = nullptr);

    QPOINTER_WRITABLE_PROPERTY(ResultsBackend, results)
    QPOINTER_WRITABLE_PROPERTY(IntersectionsBackend, backend)
};

/**
 * @class Flux
 * @brief Flux analysis module.
 *
 * Provides access to flux distribution results from the simulation.
 * Shares ResultsBackend with Intersections — both modules read from
 * the same simulation result data.
 *
 * QML access pattern: App.flux.results
 */
class Flux : public QObject {
    Q_OBJECT

public:
    Flux(QObject* parent = nullptr);

    QPOINTER_WRITABLE_PROPERTY(ResultsBackend, results)
    QPOINTER_WRITABLE_PROPERTY(FluxBackend, backend)
};

/**
 * @class Documentation
 * @brief Inline documentation registry for the SolTrace GUI.
 *
 * Loads and provides access to documentation segments displayed
 * inline alongside controls in the application.
 *
 * ## File Structure
 * Documentation is organized in a locale-aware directory hierarchy:
 * @code
 *   docs/
 *     en/
 *       configuration/
 *         sun/
 *           sun_type.md
 *           gaussian.md
 *           ...
 *         tracing/
 *           ...
 *     es/
 *       configuration/
 *         sun/
 *           sun_type.md       <- same filenames, different locale
 *           ...
 * @endcode
 *
 * ## Ordering
 * Each directory contains a manifest file (manifest.txt) that declares
 * the ordered list of files for section number derivation:
 * @code
 *   sun_type.md
 *   gaussian.md
 *   pillbox.md
 * @endcode
 * The registry walks manifests recursively at load time and assigns
 * section numbers based on traversal order. Section numbers are a
 * rendering concern — they are not stored in the files themselves.
 *
 *
 * ## Access Pattern
 * Keys follow the path schema convention using dots as separators,
 * mirroring the directory structure with slashes replaced by dots:
 * @code
 *   docs/en/configuration/sun/sun_type.md  →  "configuration.sun.sun_type"
 * @endcode
 *
 * QML access: App.docs.get("configuration.sun.sun_type")
 *
 * ## Pandoc Pipeline
 * The same directory structure is consumed by the pandoc publishing
 * pipeline to generate academic papers and technical documentation.
 * The manifest ordering determines section structure in published output.
 * Short labels for controls use Qt's tr() / .ts localization system;
 * this class handles long-form documentation body text only.
 */
class Documentation : public QObject {
    Q_OBJECT

public:
    /**
     * @param directory Root documentation directory for the active locale.
     *  e.g. ":/docs/en" or an absolute filesystem path.
     */
    Documentation(QObject* parent = nullptr, QString directory = "");

    /// True after load() has completed successfully.
    Q_READONLY_PROPERTY(bool, loaded)
    Q_WRITABLE_PROPERTY(QString, directory, "")

    /**
     * @brief Eagerly loads all documentation from the locale directory.
     *
     * Walks the directory tree using manifest.txt files in each subdirectory
     * to determine file order. Assigns section numbers based on traversal
     * order. Stores all content in m_content keyed by path-schema IDs.
     *
     * Call once at startup before QML begins binding.
     */
    void load();

    /**
     * @brief Returns the documentation body for the given key.
     * @param key Path-schema ID, e.g. "configuration.sun.sun_type"
     * @return Markdown content string, or empty string if key not found.
     */
    Q_INVOKABLE QString get(QString key);

private:
    QHash<QString, QString> m_content; /// <- key → markdown content
};

// ─── QML Entrypoint
// ───────────────────────────────────────────────────────────────────

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
    QOBJECT_READONLY_PROPERTY(FileSource, file_source)

    /// App.workflow.phases()
    QOBJECT_READONLY_PROPERTY(Workflow, workflow)

    /// App.docs.get("configuration.sun.directional_sun")
    QOBJECT_READONLY_PROPERTY(Documentation, docs)

    /// App.sun.definition.sun_type
    QOBJECT_READONLY_PROPERTY(Sun, sun)

    /// App.tracing.max_intersections
    QOBJECT_READONLY_PROPERTY(Tracing, tracing)

    /// App.materials.backend.child_model
    QOBJECT_READONLY_PROPERTY(Materials, materials)

    /// App.geometry.backend.world_geometry_model
    QOBJECT_READONLY_PROPERTY(Geometry, geometry)

    /// App.simulation.start()
    QOBJECT_READONLY_PROPERTY(SimulationModule, simulation)

    /// App.intersections.results
    QOBJECT_READONLY_PROPERTY(Intersections, intersections)

    /// App.flux.results
    QOBJECT_READONLY_PROPERTY(Flux, flux)

signals:
    void notification(ANotification);
    void new_results(std::shared_ptr<ResultDB>);
    void new_database(db::Database*);
};

} // namespace SolTrace::GUI::App
