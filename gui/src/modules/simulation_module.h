#pragma once

#include "database/database.h"
#include "job_control/job_run.h"
#include "utilities/notification.h"
#include "utilities/qt_helpers.h"
#include "utilities/structmodel.h"

#include "module_common.h"

#include <QObject>
#include <QQmlEngine>
#include <QString>

namespace SolTrace::GUI::App {

// TODO: Track added simulation results and allow deletion!

class SimulationRunnerModel;

/**
 * @class SimulationModule
 * @brief Simulation execution and progress tracking module.
 *
 * Mediates between QML controls and the job runner.
 * Exposes progress, timing metadata, and execution control to QML.
 *
 * start(), stop(), and pause() delegate to the backend after validating
 * that all required configuration modules are in a Ready or Complete state.
 *
 * QML access pattern: App.simulation.start()
 */
class SimulationModule : public QObject {
    Q_OBJECT

    QPointer<RunningJob> m_running;

    QVector<std::shared_ptr<db::SimulationResult>> m_completed_sims;

private slots:
    void job_done();

public:
    explicit SimulationModule(QObject* parent = nullptr);

    QOBJECT_WRITABLE_PROPERTY(db::Database, current_database)
    QOBJECT_READONLY_PROPERTY(StatusComponent, status);
    QOBJECT_READONLY_PROPERTY(SimulationRunnerModel, runners);

    enum Runner { CPU = 0, Embree = 1, GPU = 2 };

    Q_ENUM(Runner)

    Q_WRITABLE_PROPERTY(Runner, runner, Runner::CPU);
    Q_WRITABLE_PROPERTY(uint32_t, ray_count, 10000);
    Q_WRITABLE_PROPERTY(uint32_t, max_ray_count, 100000);
    Q_WRITABLE_PROPERTY(uint32_t, max_threads, 10);
    Q_WRITABLE_PROPERTY(uint32_t, cpu_cores, 8)
    Q_WRITABLE_PROPERTY(uint32_t, seed_value, 1234)

    Q_WRITABLE_PROPERTY(bool, sun_error, false)
    Q_WRITABLE_PROPERTY(bool, sun_shape, false)
    Q_WRITABLE_PROPERTY(bool, optical_errors, false)
    Q_WRITABLE_PROPERTY(bool, point_focus_system, false)


    /// Is a simulation being run?
    Q_READONLY_PROPERTY(bool, is_running)

    /// Ray tracing progress, 0 to 100
    Q_READONLY_PROPERTY(int, progress)

    /// e.g. "Initializing", "Ray tracing", "Complete"
    Q_READONLY_PROPERTY(QString, current_stage)
    Q_READONLY_PROPERTY(QDateTime, last_run_time)
    Q_READONLY_PROPERTY(double, elapsed_seconds)

public slots:
    void run();
    // void pause(); // no executor support for pause or resume
    // void resume();
    void cancel();

signals:
    void new_results(db::SimulationResultPtr);
    void notify(ANotification);
};

struct SimulationRunnerRecord {
    QString                  name;
    SimulationModule::Runner runner;

    RECORD_META(SimulationRunnerRecord,
                SM_EXPOSE_RO(name),
                SM_EXPOSE_RO(runner), );
};

class SimulationRunnerModel
    : public StructModelAdapter<SimulationRunnerRecord> {
    Q_OBJECT

public:
    explicit SimulationRunnerModel(QObject* parent = nullptr);

public slots:
    SimulationModule::Runner runner_at(int index) const;
    int                      index_of(SimulationModule::Runner runner) const;
};

} // namespace SolTrace::GUI::App
