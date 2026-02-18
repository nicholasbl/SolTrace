#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>
#include <qt_helpers.h>
#include <QAbstractListModel>
#include "simulation_data_api.hpp"


namespace SolTrace::GUI {

class WorkflowModel : public QObject {
    Q_OBJECT

public:
    enum class WorkflowPhase { Configuration, Simulation, Analysis };
    enum class ConfigurationSection { Sun, Tracing, Materials, Geometry};
    enum class SimulationSection {Execution, Navigation, Diagnostics};
    enum class AnalysisSection {Intersections, Flux};

    Q_ENUM(WorkflowPhase)
    Q_ENUM(ConfigurationSection)
    Q_ENUM(SimulationSection)
    Q_ENUM(AnalysisSection)

    Q_WRITABLE_PROPERTY(ConfigurationSection, configuration_section, ConfigurationSection::Sun);
    Q_WRITABLE_PROPERTY(SimulationSection, simulation_section, SimulationSection::Execution);
    Q_WRITABLE_PROPERTY(AnalysisSection, analysis_section, AnalysisSection::Intersections);

    Q_INVOKABLE static QStringList get_workflow_phases();
    Q_INVOKABLE static QStringList get_configuration_sections();
    Q_INVOKABLE static QStringList get_simulation_sections();
    Q_INVOKABLE static QStringList get_analysis_sections();

};

/*
 * ┌─────────────────────────────────────────────┐
 * │ QML (User Interface)
 * └─────────────────────────────────────────────┘
 *               │
 *              ▼
 * ┌─────────────────────────────────────────────┐
 * │ SessionManager
 * │ - Workflow state
 * │ - User input (sun, tracing, materials)
 * │ - Preset management (import/export parameters)
 * │ - Validation
 * │ - Status tracking for UX
 * └─────────────────────────────────────────────┘
 *                │
 *               ▼
 * ┌─────────────────────────────────────────────┐
 * │ Backend
 * │ - Database storage
 * │ - Simulation execution
 * │ - Results computation
 * │ - File I/O
 * └─────────────────────────────────────────────┘
 *
 */

// Hierarchy: Workflow Phase -> Workflow Section -> Workflow Module

// Responsibilities of modules:
// 1) State mediation: between the formal module state (stored in the state variable) and actual module state
// 2) Backend capability: expose and constrain and notify backend machinery
// 3) Input validation: interface between status, input handling, and system notifications
// 4) Optional serialization and import/export functionality

class ModuleBase : public QObject {
    Q_OBJECT

public:
    enum class Status { Incomplete, Loading, Ready, Error, Stale, Complete, Unset };

    Q_ENUM(Status)

    Q_WRITABLE_PROPERTY(Status, status, Status::Unset)
    Q_READONLY_PROPERTY(QString, status_message)

    void send_notification(const QString& name, const QString& body);
    void set_notification(const QString& name, const QString& body);
    void remove_notification(const QString& name);

protected:
    ModuleBase(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ModuleBase() = default;

    void mark_incomplete(const QString& message = "");
    void mark_loading(const QString& message = "");
    void mark_ready(const QString& message = "Ready");
    void mark_error(const QString& message);
    void mark_stale(const QString& message = "");
    void mark_complete(const QString& message = "");
    void mark_unset(const QString& message = "");
};

class SerializableModule;

class PresetManager : public QObject {
    Q_OBJECT

public:
    PresetManager(SerializableModule* Module, const QString& directory, QObject* parent = nullptr);
    QStringList list_presets() const;
    bool load_preset(const QString& name);
    bool save_preset(const QString& name, const QString& description = "");
    bool delete_preset(const QString& name);
    bool import_preset(const QString& filepath);
    bool export_preset(const QString& name, const QString& filepath);

private:
    QString preset_directory() const;  // Internal helper
    QPointer<SerializableModule> m_module;  // Reference to parent module
};

class SerializableModule : public ModuleBase {
    Q_OBJECT

    // Has state tracking (for loading indicators)
    // Serialization and preset management for persistence

    /*
     *  Status lifecycle for configurable data:
     *
     * Unset      - Initial status, no data loaded
     * Loading    - Actively reading preset from disk
     * Incomplete - Data loaded but missing required fields
     * Ready      - Data loaded, validated, ready for use
     * Complete   - Fully configured and simulation-ready
     * Stale      - Data valid but may be outdated (backend changed)
     * Error      - Validation failed or I/O error
     *
     */

public:

    Q_PROPERTY(PresetManager* preset_manager READ preset_manager CONSTANT)

    //─── Serialization ─────────────────────────────────────────────────

    Q_INVOKABLE virtual QJsonObject to_json() const = 0;
    Q_INVOKABLE virtual bool from_json(const QJsonObject& json) = 0;
    Q_INVOKABLE virtual bool validate() const { return true; }

protected:
    SerializableModule(QObject* parent = nullptr) : ModuleBase(parent) {}
    PresetManager* preset_manager() const { return m_preset_manager; }

private:
    QPointer<PresetManager> m_preset_manager;

};

class ReadOnlyModule : public ModuleBase {
    Q_OBJECT

    // Has state tracking (for loading indicators and error reporting)
    // But no serialization (read-only results from backend)

protected:
    ReadOnlyModule(QObject* parent = nullptr) : ModuleBase(parent) {}
};

class SunModule : public SerializableModule {
    Q_OBJECT

    enum class SunType {Directional, PointSource};
    enum class PositionCalculator {Legacy, Duffie, SOLPOS, SPA};
    enum class SunShape {Gaussian, Pillbox, CSR, Custom};

    Q_ENUM(SunType)
    Q_ENUM(PositionCalculator)
    Q_ENUM(SunShape)

    //─── Current Selections ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(SunType, sun_type, SunType::Directional)
    Q_WRITABLE_PROPERTY(PositionCalculator, position_calculator, PositionCalculator::Legacy)
    Q_WRITABLE_PROPERTY(SunShape, sun_shape, SunShape::Gaussian)

    //─── Directional Sun Properties ───────────────────────────────────────────────────────────────────

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
    Q_WRITABLE_PROPERTY(int, interval, 1) // seconds

    // SPA Optional fields
    Q_WRITABLE_PROPERTY(bool, optional_spa_fields, false)
    Q_WRITABLE_PROPERTY(double, dut1, 0.0)
    Q_WRITABLE_PROPERTY(double, altitude, 1000)
    Q_WRITABLE_PROPERTY(double, pressure, 1013.25)
    Q_WRITABLE_PROPERTY(double, temperature, 20.0)

    //─── Point Source Properties ───────────────────────────────────────────────────────────────────

    // Position
    Q_WRITABLE_PROPERTY(double, x, 1000)
    Q_WRITABLE_PROPERTY(double, y, 1000)
    Q_WRITABLE_PROPERTY(double, z, 1000)

    //─── Emission Profile ───────────────────────────────────────────────────────────────────

    // Gaussian
    Q_WRITABLE_PROPERTY(double, std, 5.18)
    // Pillbox
    Q_WRITABLE_PROPERTY(double, half_width, 4.65)
    // Buie
    Q_WRITABLE_PROPERTY(double, csr, 2.0)
    // Custom Sun Shape
    Q_WRITABLE_PROPERTY(int, num_points, 1)

    Q_PROPERTY(QAbstractListModule* custom_profile READ custom_profile NOTIFY custom_profile_changed)

public:
    SunModule(QObject* parent = nullptr);

    //─── Serialization Overrides ───────────────────────────────────────────────────────────────────

    Q_INVOKABLE QJsonObject to_json() const override;
    Q_INVOKABLE bool from_json(const QJsonObject& json) override;
};

class TracingModule : public SerializableModule {
    Q_OBJECT

    enum class Tracer {Legacy, Refactored, GPU};

    Q_ENUM(Tracer)

    Q_WRITABLE_PROPERTY(Tracer, tracer, Tracer::GPU)

    //─── Engine Properties ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(double, max_intersections, 3.5e6)
    Q_WRITABLE_PROPERTY(double, max_rays, 1e6)
    Q_WRITABLE_PROPERTY(bool, sun_shape, false)

    //─── Execution Properties ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(int, cpu_cores, 4)
    Q_WRITABLE_PROPERTY(int, seed_value, 12345)

    //─── Optimization Properties ───────────────────────────────────────────────────────────────────

    Q_WRITABLE_PROPERTY(bool, optical_errors, false)
    Q_WRITABLE_PROPERTY(bool, point_focus_system, false)

public:
    TracingModule(QObject* parent = nullptr);

    //─── Serialization Overrides ───────────────────────────────────────────────────────────────────

    Q_INVOKABLE QJsonObject to_json() const override;
    Q_INVOKABLE bool from_json(const QJsonObject& json) override;
};


class MaterialsModule : public SerializableModule {
    Q_OBJECT

    // backend: db
public:
    MaterialsModule(QObject* parent = nullptr);

    //─── Serialization Overrides ───────────────────────────────────────────────────────────────────

    Q_INVOKABLE QJsonObject to_json() const override;
    Q_INVOKABLE bool from_json(const QJsonObject& json) override;
};


class GeometryModule : public SerializableModule {
    Q_OBJECT

    // backend: db
public:
    GeometryModule(QObject* parent = nullptr);

    //─── Serialization Overrides ───────────────────────────────────────────────────────────────────

    Q_INVOKABLE QJsonObject to_json() const override;
    Q_INVOKABLE bool from_json(const QJsonObject& json) override;
};

class SimulationRunnerModule : public ReadOnlyModule {
    Q_OBJECT

    // - tracks status
    // - prevents running of sim with significant errors in sim config
    // - sim metadata etc
    // - error/warning reporting
    // - sim exposed progress here

    // Validation
    Q_INVOKABLE bool can_run_simulation() const;
    Q_READONLY_PROPERTY(QStringList, errors)
    Q_READONLY_PROPERTY(QStringList, warnings)

    // Execution control
    Q_INVOKABLE void start_simulation();
    Q_INVOKABLE void cancel_simulation();
    Q_INVOKABLE void pause_simulation();

    // Progress tracking
    Q_READONLY_PROPERTY(double, progress)  // 0.0 to 1.0
    Q_READONLY_PROPERTY(int, rays_traced)
    Q_READONLY_PROPERTY(QString, current_stage)  // "Initializing", "Ray tracing", etc.

    // Metadata
    Q_READONLY_PROPERTY(QDateTime, last_run_time)
    Q_READONLY_PROPERTY(double, elapsed_seconds)
    Q_READONLY_PROPERTY(int, total_rays)

public:
    SimulationRunnerModule(QObject* parent = nullptr);

    void on_simulation_started();
    void on_progress_update(double progress, int rays_traced);
    void on_simulation_completed();
    void on_simulation_error(const QString& error);
};

class CameraModule : public SerializableModule {
    Q_OBJECT


    // backend: list model
    // list item: camera position + orientation
public:
    CameraModule(QObject* parent = nullptr);

    //─── Serialization Overrides ───────────────────────────────────────────────────────────────────

    Q_INVOKABLE QJsonObject to_json() const override;
    Q_INVOKABLE bool from_json(const QJsonObject& json) override;
};

class IntersectionModule : public ReadOnlyModule {
    Q_OBJECT

    // backend: db
public:
    IntersectionModule(QObject* parent = nullptr);
};

class FluxModule : public ReadOnlyModule {
    Q_OBJECT

    // backend: db
public:
    FluxModule(QObject* parent = nullptr);
};


class SessionManager : public QObject {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
public:
    SessionManager(QObject* parent = nullptr);

    //─── Workflow Representation ───────────────────────────────────────────────────────────────────

    QOBJECT_READONLY_PROPERTY(WorkflowModel, workflow);

    //─── Configuration Module Data ───────────────────────────────────────────────────────────────────
    // Binds to SimulationData backend

    QOBJECT_READONLY_PROPERTY(SunModule, sun);
    QOBJECT_READONLY_PROPERTY(TracingModule, tracing);
    QOBJECT_READONLY_PROPERTY(MaterialsModule, materials);
    QOBJECT_READONLY_PROPERTY(GeometryModule, geometry);

    //─── Simulation Module Data ───────────────────────────────────────────────────────────────────

    QOBJECT_READONLY_PROPERTY(SimulationRunnerModule, simulation);
    QOBJECT_READONLY_PROPERTY(CameraModule, cameras);

    //─── Analysis Module Data ───────────────────────────────────────────────────────────────────

    QOBJECT_READONLY_PROPERTY(IntersectionModule, intersections);
    QOBJECT_READONLY_PROPERTY(FluxModule, flux);

};

}

